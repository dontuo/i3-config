#!/bin/sh
set -e

# Get directory where this script is located
SCRIPT_DIR=$(cd -- "$(dirname -- "$0")" && pwd)

# ==========================================
# 1. PRE-CONFIGURATION
# ==========================================
echo "==> Setup Configuration"

DETECTED_REPO=$(git -C "$SCRIPT_DIR" config --get remote.origin.url || echo "")

read -rp "Enter hostname: " HOSTNAME_VAR

if [ -z "$DETECTED_REPO" ]; then
    read -rp "Enter Dotfiles Git URL: " DOTFILES_REPO
else
    read -rp "Confirm Dotfiles Git URL [$DETECTED_REPO]: " INPUT_REPO
    DOTFILES_REPO="${INPUT_REPO:-$DETECTED_REPO}"
fi

echo "Enter password for ROOT:"
read -rs ROOT_PASS
echo "Enter password for USER 'user':"
read -rs USER_PASS
echo "Enter password for USER 'work':"
read -rs WORK_PASS

echo ""
echo "==> Configuration saved. Starting installation..."

# ==========================================
# 2. SYSTEM BASE
# ==========================================

echo "==> Timezone & Clock"
ln -sf /usr/share/zoneinfo/Europe/Kyiv /etc/localtime
hwclock --systohc

echo "==> Locale"
sed -i 's/^#en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen
locale-gen
echo "LANG=en_US.UTF-8" > /etc/locale.conf

echo "==> Hostname"
echo "$HOSTNAME_VAR" > /etc/hostname

echo "==> Enabling Multilib Repo (32-bit support)"
sed -i "/\[multilib\]/,/Include/"'s/^#//' /etc/pacman.conf
pacman -Sy

echo "==> Installing Base Packages"
# 'go' is helpful for yay compilation
pacman -S --noconfirm \
  grub efibootmgr sudo fish \
  networkmanager vim git stow base-devel go

echo "==> Installing Official Packages (packages.txt)"
if [ -f "$SCRIPT_DIR/packages.txt" ]; then
    grep -vE '^\s*#|^\s*$' "$SCRIPT_DIR/packages.txt" | pacman -S --needed -
else
    echo "WARNING: packages.txt not found in $SCRIPT_DIR"
fi

echo "==> Enable NetworkManager"
systemctl enable NetworkManager

echo "==> Install GRUB"
grub-install --target=x86_64-efi --efi-directory=/efi --bootloader-id=GRUB
grub-mkconfig -o /boot/grub/grub.cfg

# ==========================================
# 3. USERS & PRIVILEGES
# ==========================================

echo "==> Creating Users"
id "user" &>/dev/null || useradd -m -G wheel -s /bin/fish user
id "work" &>/dev/null || useradd -m -G wheel -s /bin/fish work

echo "==> Configuring Sudo (Temporary NOPASSWD)"
# Allow wheel to sudo without password temporarily for yay installation
echo "%wheel ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/temp_install

# ==========================================
# 4. INSTALL YAY & AUR PACKAGES
# ==========================================

echo "==> Installing Yay (AUR Helper)"
if ! command -v yay &> /dev/null; then
    cd /opt
    git clone https://aur.archlinux.org/yay.git
    chown -R user:user /opt/yay
    cd /opt/yay
    # Build as 'user'
    sudo -u user makepkg -si --noconfirm
    cd ..
    rm -rf yay
else
    echo "    Yay is already installed."
fi

echo "==> Installing AUR Packages (packages_aur.txt)"
if [ -f "$SCRIPT_DIR/packages_aur.txt" ]; then
    echo "    installing packages_aur.txt..."
    grep -vE '^\s*#|^\s*$' "$SCRIPT_DIR/packages_aur.txt" | sudo -u user yay -S --noconfirm --needed -
else
    echo "WARNING: packages_aur.txt not found in $SCRIPT_DIR"
fi

# ==========================================
# 5. DOTFILES SETUP
# ==========================================

setup_user_env() {
    local USERNAME=$1
    echo "--> Setting up environment for: $USERNAME"

    local TARGET_DIR="/home/$USERNAME/dotfiles"
    
    # 1. Clone/Pull Dotfiles
    if [ -d "$TARGET_DIR" ]; then
        echo "    Updating dotfiles..."
        sudo -u "$USERNAME" git -C "$TARGET_DIR" pull
    else
        echo "    Cloning dotfiles..."
        sudo -u "$USERNAME" git clone --recursive "$DOTFILES_REPO" "$TARGET_DIR"
    fi

    # 2. Setup Ignore file
    echo "install.sh" > "$TARGET_DIR/.stow-local-ignore"
    echo "packages.txt" >> "$TARGET_DIR/.stow-local-ignore"
    echo "packages_aur.txt" >> "$TARGET_DIR/.stow-local-ignore"
    echo ".git" >> "$TARGET_DIR/.stow-local-ignore"
    echo "README.md" >> "$TARGET_DIR/.stow-local-ignore"
    chown "$USERNAME:$USERNAME" "$TARGET_DIR/.stow-local-ignore"

    # 3. Stow
    echo "    Stowing configuration..."
    sudo -u "$USERNAME" sh -c "cd $TARGET_DIR && stow ."
    #
    # 4. Fish Auto-start
    local FISH_CONFIG_DIR="/home/$USERNAME/.config/fish"
    sudo -u "$USERNAME" mkdir -p "$FISH_CONFIG_DIR"
    local FISH_CONF="$FISH_CONFIG_DIR/config.fish"
    
    sudo -u "$USERNAME" touch "$FISH_CONF"
    
    if ! grep -q "startx" "$FISH_CONF"; then
        {
            echo "" 
            echo '# Auto-start X on TTY1' 
            echo 'if status is-login' 
            echo '    if test -z "$DISPLAY" -a "$XDG_VTNR" = 1' 
            echo '        exec startx -- -keeptty' 
            echo '    end' 
            echo 'end'
        } >> "$FISH_CONF"
    fi
    
}

install_programs_from_source()
{
    echo "==> Installing programs from source..."

    # 1. ENSURE SUBMODULES (Just in case boomer or others are submodules)
    # If dmenu is just a regular folder, this won't hurt it.
    if [ -d "$SCRIPT_DIR/.git" ]; then
        echo "    Initializing git submodules..."
        git config --global --add safe.directory "$SCRIPT_DIR"
        git -C "$SCRIPT_DIR" submodule update --init --recursive
    fi

    # 2. INSTALL YOUR PATCHED DMENU
    # We look for the folder 'dmenu' inside your repo
    if [ -d "$SCRIPT_DIR/dmenu" ]; then
        echo "    Compiling your patched dmenu..."
        cd "$SCRIPT_DIR/dmenu"
        
        # Check if config.mk exists (standard for suckless tools)
        if [ -f "config.mk" ]; then
            # Clean and Install
            # sudo is not needed here because the script is already running as root
            make clean install
            echo "    Dmenu installed."
        else
            echo "WARNING: No 'config.mk' found in dmenu folder. Is it the source code?"
        fi
    else
        echo "WARNING: '$SCRIPT_DIR/dmenu' folder not found. Skipping dmenu."
    fi

    # 3. INSTALL BOOMER
    if [ -d "$SCRIPT_DIR/boomer" ]; then
        echo "    Compiling boomer..."
        cd "$SCRIPT_DIR/boomer"

        if command -v nimble >/dev/null; then
            # Clean previous builds just in case
            rm -f boomer
            
            # Build release version
            nimble build -d:release --accept
            
            # Install manually to /usr/local/bin
            if [ -f "boomer" ]; then
                cp boomer /usr/local/bin/
                chmod 755 /usr/local/bin/boomer
                echo "    Boomer installed to /usr/local/bin."
            else
                echo "ERROR: Boomer binary not found after build."
            fi
        else
            echo "WARNING: 'nimble' not found. Is 'nim' installed?"
        fi
    else
        echo "WARNING: '$SCRIPT_DIR/boomer' folder not found. Skipping."
    fi
}

setup_user_env "user"
setup_user_env "work"
install_programs_from_source

# ==========================================
# 6. FINALIZATION
# ==========================================

echo "==> Reverting Sudo to Password Required"
rm /etc/sudoers.d/temp_install
# Standard Arch sudo uncomment (Password required)
sed -i 's/^# %wheel ALL=(ALL:ALL) ALL/%wheel ALL=(ALL:ALL) ALL/' /etc/sudoers

echo "==> Applying passwords"
echo "root:$ROOT_PASS" | chpasswd
echo "user:$USER_PASS" | chpasswd
echo "work:$WORK_PASS" | chpasswd
unset ROOT_PASS USER_PASS WORK_PASS

echo "==> Done. Reboot when ready."
