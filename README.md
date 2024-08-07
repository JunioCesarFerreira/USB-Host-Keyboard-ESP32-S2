# USB Host Keyboard ESP32-S2

🌍 *[Português](README.md) ∙ [English](README_en.md)*

## Visão Geral
Este projeto implementa um host de teclado USB utilizando o microcontrolador ESP32-S2. Elaborado para interagir com teclados USB e processar suas entradas de forma eficiente.

## Recursos
- **Funcionalidade de host de teclado USB**: Permite a conexão e interação direta com teclados USB.
- **Compatível com ESP32-S2**: Projetado especificamente para o microcontrolador ESP32-S2, aproveitando suas capacidades.
- **Design modular**: Componentes separados para fácil personalização e expansão.

## Primeiros Passos

### Pré-requisitos
- Microcontrolador **ESP32-S2**
- Teclado USB

### Instalação
1. Instale o **PlatformIO IDE** ou integre-o ao **Visual Studio Code**.
2. Clone este repositório para sua máquina local:
   ```bash
   git clone https://github.com/seu_usuario/usb-host-keyboard-esp32-s2.git
   ```
3. Abra o projeto no PlatformIO ou no ambiente de desenvolvimento ESP32 de sua preferência.

### Uso
1. Conecte seu teclado USB ao microcontrolador ESP32-S2.
2. Ligue o ESP32-S2.
3. O dispositivo começará automaticamente a processar as entradas do teclado.

## Estrutura do Projeto
- **`.gitignore`**: Lista de arquivos e diretórios ignorados pelo Git.
- **`.pio`**: Armazena saídas e configurações de build do PlatformIO.
- **`.vscode`**: Arquivos de configuração do VSCode para o projeto.
- **`include`**: Arquivos de cabeçalho e inclusão.
- **`lib`**: Bibliotecas utilizadas no projeto.
  - **RgbPixel**: Controle de pixels RGB.
  - **USBHostHid**: Manipulação de HID Host USB.
  - **USBKeyboard**: Funcionalidades de teclado USB.
- **`platformio.ini`**: Arquivo de configuração do PlatformIO.
- **`src`**: Código fonte principal do projeto.
- **`test`**: Scripts de teste para validar o projeto.

## Bibliotecas
- **RgbPixel**: Gerencia o controle de pixels RGB.
- **USBHostHid**: Gerencia funcionalidades de HID Host USB.
- **USBKeyboard**: Implementa a interação com teclados USB.

## Contribuindo
Contribuições são bem-vindas! Siga as práticas padrão de codificação e forneça descrições detalhadas das suas alterações no projeto. Para contribuir, por favor:
1. Fork este repositório.
2. Crie uma nova branch para suas mudanças:
   ```bash
   git checkout -b minha-nova-feature
   ```
3. Commit suas alterações:
   ```bash
   git commit -m "Adiciona nova feature"
   ```
4. Push para a branch:
   ```bash
   git push origin minha-nova-feature
   ```
5. Abra um Pull Request para revisão.

## Licença
Este projeto está licenciado sob a [MIT License](LICENSE).