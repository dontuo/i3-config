#!/bin/bash
set -e

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
echo "==> Configuration saved. Starting Installation..."

# ==========================================
# 2. PORTAGE CONFIGURATION
# ==========================================
echo "--> Installing Portage Config Files..."

if [ -f "$SCRIPT_DIR/make.conf" ]; then
    cp "$SCRIPT_DIR/make.conf" /etc/portage/make.conf
else
    echo "ERROR: make.conf not found."
    exit 1
fi

# Clean and recreate config directories
for dir in package.use package.accept_keywords package.license; do
    rm -rf "/etc/portage/$dir"
    mkdir -p "/etc/portage/$dir"
    if [ -f "$SCRIPT_DIR/$dir" ]; then
        cp "$SCRIPT_DIR/$dir" "/etc/portage/$dir/custom"
    fi
done

echo "--> Configuring Binary Repository..."
mkdir -p /etc/portage/binrepos.conf
cat <<EOF > /etc/portage/binrepos.conf/gentoobinhost.conf
[gentoobinhost]
priority = 1
sync-uri = https://gentoo.osuosl.org/releases/amd64/binpackages/23.0/x86-64/
EOF

echo "--> Syncing Repositories..."
emerge-webrsync
emerge --sync

echo "--> Selecting Profile..."
eselect profile set default/linux/amd64/23.0/desktop

echo "--> Updating @world..."
# FIX APPLIED HERE:
# 1. Added --autounmask-backtrack=y (Allows deep dependency calculation)
# 2. Added --autounmask-keep-masks=y (Prevents unrelated unmasking issues)
CMD_ARGS="--verbose --update --deep --newuse -gk --binpkg-respect-use=n --autounmask-write --autounmask-backtrack=y --autounmask-keep-masks=y"

emerge $CMD_ARGS @world || {
    echo "!! Emerge failed. Applying config changes and retrying..."
    yes | etc-update --automode -5
    emerge $CMD_ARGS @world
}

# ==========================================
# 3. INSTALL PACKAGES
# ==========================================
echo "--> Enabling GURU Overlay..."
emerge -gk --binpkg-respect-use=n app-eselect/eselect-repository
eselect repository enable guru
emaint sync -r guru

echo "--> Installing Packages..."
if [ -f "$SCRIPT_DIR/packages_world" ]; then
    PACKAGES=$(grep -vE '^\s*#|^\s*$' "$SCRIPT_DIR/packages_world")
    
    # Applied the same backtrack flag here
    echo "$PACKAGES" | xargs emerge --ask=n --verbose --keep-going -gk --binpkg-respect-use=n --autounmask-continue --autounmask-backtrack=y
else
    echo "WARNING: packages_world file not found!"
fi

# ==========================================
# 4. SYSTEM CONFIGURATION
# ==========================================
echo "--> Timezone & Locale..."
echo "Europe/Kyiv" > /etc/timezone
emerge --config sys-libs/timezone-data
echo "en_US.UTF-8 UTF-8" >> /etc/locale.gen
locale-gen
eselect locale set en_US.utf8

echo "--> Hostname..."
echo "hostname=\"$HOSTNAME_VAR\"" > /etc/conf.d/hostname

echo "--> Passwords..."
echo "root:$ROOT_PASS" | chpasswd

echo "--> Services..."
rc-update add NetworkManager default
rc-update add dbus default
rc-update add elogind boot

# ==========================================
# 5. USERS & DOTFILES
# ==========================================
setup_user() {
    local USERNAME=$1
    local PASSWORD=$2
    local REPO_URL=$3

    echo "--> Setting up user: $USERNAME"
    id "$USERNAME" &>/dev/null || useradd -m -G wheel,audio,video,usb,input -s /bin/fish "$USERNAME"
    echo "$USERNAME:$PASSWORD" | chpasswd
    
    local TARGET_DIR="/home/$USERNAME/dotfiles"
    
    if [ ! -z "$REPO_URL" ]; then
        if [ -d "$TARGET_DIR" ]; then
            echo "    Updating dotfiles..."
            sudo -u "$USERNAME" git -C "$TARGET_DIR" pull
        else
            echo "    Cloning dotfiles..."
            sudo -u "$USERNAME" git clone --recursive "$REPO_URL" "$TARGET_DIR"
        fi
        
        # Add ignores
        for f in install_gentoo.sh make.conf package.use package.accept_keywords package.license packages_world; do
            echo "$f" >> "$TARGET_DIR/.stow-local-ignore"
        done
        chown "$USERNAME:$USERNAME" "$TARGET_DIR/.stow-local-ignore"
        
        echo "    Stowing..."
        sudo -u "$USERNAME" sh -c "cd $TARGET_DIR && stow ."
    fi
}

setup_user "user" "$USER_PASS" "$DOTFILES_REPO"
setup_user "work" "$WORK_PASS" "$DOTFILES_REPO"
unset ROOT_PASS USER_PASS WORK_PASS

echo "--> Sudo..."
mkdir -p /etc/sudoers.d
echo '%wheel ALL=(ALL:ALL) ALL' > /etc/sudoers.d/wheel
chmod 440 /etc/sudoers.d/wheel

# ==========================================
# 6. BOOTLOADER
# ==========================================
echo "--> GRUB..."
grub-install --target=x86_64-efi --efi-directory=/boot
grub-mkconfig -o /boot/grub/grub.cfg

echo "==> Done. Reboot when ready."
