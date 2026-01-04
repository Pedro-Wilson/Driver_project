#!/bin/bash

# 1. Limpeza Profunda
echo "--- Limpando ambiente e desligando driver anterior ---"
sudo rmmod pico_driver 2>/dev/null  # Desliga o driver
make clean                         # Remove binários antigos
sudo rm -f /dev/pico_usb0          # Remove o nó antigo, se existir

# 2. Compilação
echo "--- Compilando Driver e Aplicação ---"
make CC=gcc-13 LD=ld ARCH=x86_64 CROSS_COMPILE= -C /lib/modules/$(uname -r)/build M=$(pwd) modules
gcc user_app.c -o user_app

# 3. Carregamento do Módulo (Liga o driver)
echo "--- Carregando o Driver Pico USB ---"
sudo insmod pico_driver.ko

if [ $? -eq 0 ]; then
    # 4. PERFUMARIA: Notificação Visual Real
    # Usando suas variáveis específicas (:1 e seu xauth)
    export DISPLAY=:1
    export XAUTHORITY=/run/user/1000/xauth_fUSLpc
    
    notify-send -i drive-harddisk-usb \
    "USB Pico Conectado" \
    "Dispositivo virtual registrado em /dev/pico_usb0"
    
    echo "--- Notificação enviada ao sistema ---"
fi

# 5. Ajustes de Permissão
# O Kernel cria o nó via device_create, mas precisamos de tempo para o udev processar
sleep 1
sudo chmod 666 /dev/pico_usb0

# 6. Execução da Aplicação
echo "--- Executando Aplicação de Usuário ---"
./user_app

# 7. Logs em tempo real
echo "--- Monitorando Logs do Kernel (Ctrl+C para sair) ---"
# O -c limpa o buffer do dmesg antes de começar, assim você vê apenas os logs de AGORA
sudo dmesg -c > /dev/null
sudo dmesg -w | grep "PICO USB"