#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/device.h>
#include <linux/kmod.h>     // Necessário para call_usermodehelper

#define DEVICE_NAME "pico_usb0"
#define CLASS_NAME  "pico"
#define MY_TIMEOUT  2000 

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pedro Wilson");
MODULE_DESCRIPTION("Driver TPSE II - Simulação Pico USB com Notificação Visual");

static int majorNumber;
static char message[256] = {0};
static struct timer_list timer0;
static int counter = 0;
static struct class* pico_class  = NULL; 
static struct device* pico_device = NULL; 

// Função para disparar notificação visual no Desktop
static void send_desktop_notification(void) {
    char *envp[] = { 
        "HOME=/", 
        "DISPLAY=:0", 
        "XAUTHORITY=/run/user/1000/gdm/Xauthority", // Padrão Ubuntu
        NULL 
    };
    char *argv[] = { 
        "/usr/bin/notify-send", 
        "-i", "drive-harddisk-usb", 
        "Pico USB Conectado", 
        "O dispositivo virtual /dev/pico_usb0 está ativo.", 
        NULL 
    };

    // Executa o comando no espaço de usuário a partir do kernel
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
}

static void timer_callback(struct timer_list *t) {
    pr_info("[PICO USB] Evento de Hardware Simulado. Contador: %d\n", counter++);
    mod_timer(&timer0, jiffies + msecs_to_jiffies(MY_TIMEOUT));
}

static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("[PICO USB] Dispositivo aberto pela aplicacao.\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    int size_of_message;
    if (*offset > 0) return 0;

    sprintf(message, "Pico respondendo! Contador de interrupcoes: %d\n", counter);
    size_of_message = strlen(message);
    error_count = copy_to_user(buffer, message, size_of_message);

    if (error_count == 0) {
        *offset += size_of_message;
        return size_of_message;
    } else return -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    size_t copy_len = (len < 255) ? len : 255;
    memset(message, 0, sizeof(message));
    if (copy_from_user(message, buffer, copy_len)) return -EFAULT;
    pr_info("[PICO USB] Dados recebidos do Host: %s\n", message);
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("[PICO USB] Dispositivo liberado.\n");
    return 0;
}

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init pico_driver_init(void) {
    pr_info("[PICO USB] Inicializando subsistema...\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) return majorNumber;

    pico_class = class_create(CLASS_NAME);
    if (IS_ERR(pico_class)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        return PTR_ERR(pico_class);
    }

    pico_device = device_create(pico_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(pico_device)) {
        class_destroy(pico_class);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        return PTR_ERR(pico_device);
    }

    timer_setup(&timer0, timer_callback, 0);
    mod_timer(&timer0, jiffies + msecs_to_jiffies(MY_TIMEOUT));

    // Dispara a notificação visual ao carregar
    send_desktop_notification();

    pr_info("[PICO USB] Dispositivo conectado virtualmente e pronto!\n");
    return 0;
}

static void __exit pico_driver_exit(void) {
    del_timer(&timer0);
    device_destroy(pico_class, MKDEV(majorNumber, 0));
    class_destroy(pico_class);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    pr_info("[PICO USB] Dispositivo desconectado. Driver encerrado.\n");
}

module_init(pico_driver_init);
module_exit(pico_driver_exit);