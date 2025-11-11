#!/bin/sh
set -e

HOOK_SRC="./.githooks/commit-msg"
HOOK_DIR="./.git/hooks"

echo "🔧 Setting up development environment..."

# Detect package manager
if command -v apt >/dev/null; then
    PKG_INSTALL="sudo apt install -y"
elif command -v dnf >/dev/null; then
    PKG_INSTALL="sudo dnf install -y"
elif command -v brew >/dev/null; then
    PKG_INSTALL="brew install"
else
    echo "❌ No supported package manager found (apt/dnf/brew)."
    exit 1
fi

# Install core tools
echo "📦 Installing required tools..."
$PKG_INSTALL clang-format clang-tidy pre-commit bear

# Install commit-msg hooks
if [ -f "$HOOK_SRC" ]; then
    mkdir -p "$HOOK_DIR"
    DEST="$HOOK_DIR/commit-msg"
    if [ -f "$DEST" ]; then
        if cmp -s "$HOOK_SRC" "$DEST"; then
            echo "✅ commit-msg hook already up to date."
        else
            cp -f "$HOOK_SRC" "$DEST"
            chmod +x "$DEST"
            echo "🔐 commit-msg hook updated."
        fi
    else
        cp "$HOOK_SRC" "$DEST"
        chmod +x "$DEST"
        echo "🔐 commit-msg hook installed."
    fi
else
    echo "ℹ️ commit-msg hook not found at $HOOK_SRC, skipping."
fi

# Install pre-commit hooks
echo "🔗 Setting up Git hooks..."
pre-commit install

echo "✅ Setup complete!"
