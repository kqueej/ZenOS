# ZenOS
![2](https://github.com/user-attachments/assets/47cff0b6-14a5-44d6-9dda-248524830bcb)


- [RU README](https://github.com/Junsious/zen-kernel-OS/blob/main/README_ru.md)

### Requirements

- **Cross-compiler**: `i386-elf-gcc` and `i386-elf-as`
- **ISO Image Creation Tool**: `grub-mkrescue`
- **Emulator**: `QEMU`

### Build

1. **Clone the repository**:

    ```bash
    git clone https://github.com/yourusername/ZenOS.git
    cd zen-kernel-OS
    ```

2. **Build the project**:

    ```bash
    make
    ```

3. **Run the ISO image with QEMU**:

    ```bash
    qemu-system-i386 -cdrom ZenOS.iso / qemu-system-i386 -cdrom ZenOS.bin -curses 
    ```
