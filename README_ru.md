# ZenOS
![2](https://github.com/user-attachments/assets/47cff0b6-14a5-44d6-9dda-248524830bcb)

- [ENG_README](https://github.com/Junsious/zen-kernel-OS/blob/main/README.md)

### Требования

- **Кросс-компилятор**: `i386-elf-gcc` и `i386-elf-as`
- **Утилита для создания ISO-образов**: `grub-mkrescue`

### Сборка

1. **Клонируйте репозиторий**:

    ```bash
    git clone https://github.com/yourusername/zen-kernel-OS.git
    cd zen-kernel-OS
    ```

2. **Соберите проект**:

    ```bash
    make
    ```

3. **Запустите ISO-образ с помощью QEMU**:

    ```bash
    qemu-system-i386 -cdrom ZenOS.iso / qemu-system-i386 -cdrom ZenOS.bin -curses 
    ```
