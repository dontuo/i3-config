#!/bin/bash
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
echo "==> Configuration saved. Starting Binary-First Installation..."

# ==========================================
# 2. PORTAGE CONFIGURATION
# ==========================================
echo "--> Installing Portage Config Files..."

# 1. make.conf
if [ -f "$SCRIPT_DIR/make.conf" ]; then
    cp "$SCRIPT_DIR/make.conf" /etc/portage/make.conf
else
    echo "ERROR: make.conf not found."
    exit 1
fi

# 2. package.use
if [ -d /etc/portage/package.use ]; then rm -rf /etc/portage/package.use; fi
mkdir -p /etc/portage/package.use
if [ -f "$SCRIPT_DIR/package.use" ]; then
    cp "$SCRIPT_DIR/package.use" /etc/portage/package.use/custom
fi

# 3. package.accept_keywords
if [ -d /etc/portage/package.accept_keywords ]; then rm -rf /etc/portage/package.accept_keywords; fi
mkdir -p /etc/portage/package.accept_keywords
if [ -f "$SCRIPT_DIR/package.accept_keywords" ]; then
    cp "$SCRIPT_DIR/package.accept_keywords" /etc/portage/package.accept_keywords/custom
fi

# 4. package.license
if [ -d /etc/portage/package.license ]; then rm -rf /etc/portage/package.license; fi
mkdir -p /etc/portage/package.license
if [ -f "$SCRIPT_DIR/package.license" ]; then
    cp "$SCRIPT_DIR/package.license" /etc/portage/package.license/custom
fi

echo "--> Configuring Binary Repository (Binhost)..."
mkdir -p /etc/portage/binrepos.conf
cat <<EOF > /etc/portage/binrepos.conf/gentoobinhost.conf
[gentoobinhost]
priority = 1
sync-uri = https://gentoo.osuosl.org/releases/amd64/binpackages/23.0/x86-64/
EOF

echo "--> Syncing Repositories..."
emerge-webrsync
emerge --sync

echo "--> Selecting Profile (Desktop OpenRC)..."
eselect profile set default/linux/amd64/23.0/desktop

echo "--> Updating @world (Using Binaries)..."
# Updated logic:
# 1. Try to install with binaries, respecting configs.
# 2. If it fails (usually due to config needs), write changes (--autounmask-write).
# 3. Use 'etc-update' to auto-accept those changes (-5 = auto-accept).
# 4. Try again.
emerge --verbose --update --deep --newuse -gk --binpkg-respect-use=n --autounmask-write @world || {
    echo "!! First pass failed. Auto-accepting configuration changes..."
    # -5 means: automatically merge trivial changes, force update others
    yes | etc-update --automode -5
    echo "!! Retrying update..."
    emerge --verbose --update --deep --newuse -gk --binpkg-respect-use=n @world
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
    
    # Updated: Added --binpkg-respect-use=n to force binary usage
    echo "$PACKAGES" | xargs emerge --ask=n --verbose --keep-going -gk --binpkg-respect-use=n --autounmask-continue
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
