# 🚀 Driver USB Simulator - Raspberry Pi Pico (TPSE II)

Este projeto consiste no desenvolvimento de um Driver de Dispositivo de Caractere (LKM - Linux Kernel Module) que simula o comportamento de hardware de uma Raspberry Pi Pico integrada ao Kernel do Linux. 

<img width="521" height="196" alt="ccc" src="https://github.com/user-attachments/assets/3e9bea6c-5f7a-4cf7-91aa-913ac171ad7d" />


## 📋 Sobre o Projeto

O sistema simula a comunicação Host-Hardware através de um arquivo de dispositivo localizado em `/dev/pico_usb0`. O driver utiliza interrupções de software (Kernel Timers) para manter um contador persistente que representa o clock interno do hardware simulado.

### Principais Funcionalidades:

- **Kernel Timer**: Um contador assíncrono que incrementa em background (1Hz)
- **Comunicação Bidirecional**: Implementação completa das System Calls `read()` e `write()`
- **Persistência**: O "hardware" continua processando dados mesmo quando nenhuma aplicação de usuário está aberta
- **Notificações do Sistema**: Integração com o ambiente desktop via `notify-send`

## 🏗️ Arquitetura do Sistema

O projeto é dividido em duas camadas principais:

- **Kernel Space** (`pico_driver.c`): Onde reside a lógica do driver, gerenciamento de memória e timers
- **User Space** (`user_app.c`): Aplicação de interface que consome os dados do driver e envia comandos de controle

## 🛠️ Como Executar

### Pré-requisitos

- Ambiente Linux (Ubuntu/Debian recomendado)
- Kernel Headers instalados
- Compilador gcc e ferramenta make

### Passo a Passo

1. **Clonar o repositório:**
   ```bash
   git clone https://github.com/Pedro-Wilson/Driver_USB.git
   cd Driver_USB
   ```
2. Executar o Script de Automação:
O arquivo start.sh gerencia automaticamente a limpeza, compilação, carregamento do módulo e execução da interface.


```bash
chmod +x start.sh
./start.sh
```

<img width="231" height="132" alt="dddd" src="https://github.com/user-attachments/assets/c60a3aa3-b737-45f6-9909-8390d93cabad" />


## 🖥️ Fluxo de Demonstração

Ao iniciar o projeto via `start.sh`, o seguinte fluxo ocorre:

1. **Compilação**: O módulo `pico_driver.ko` é gerado
2. **Carga do Módulo**: O driver é inserido no kernel (`insmod`) e o nó `/dev/pico_usb0` é criado
3. **Interface de Usuário**: A aplicação inicia um ciclo de 10 leituras em tempo real. Você verá o contador subindo no terminal
4. **Monitoramento do Kernel**: Após as 10 leituras, o terminal entra em modo de monitoramento (`dmesg -w`), onde é possível observar o contador continuando a subir infinitamente no espaço do Kernel, provando a independência do driver

## 📄 Detalhes Técnicos (System Calls)

| Função | Comportamento no Driver |
|--------|------------------------|
| `dev_read` | Retorna o valor atual do contador de hardware para o usuário |
| `dev_write` | Recebe strings do usuário e as registra nos logs do Kernel |
| `timer_callback` | Função acionada a cada 1000ms para simular o clock do dispositivo |
