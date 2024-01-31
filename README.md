# USB Host Keyboard ESP32-S2

🌍 *[Português](README.md) ∙ [English](README_en.md)*

## Visão Geral
Este projeto implementa um teclado USB host usando um microcontrolador ESP32-S2. Ele é projetado para interagir com teclados USB e processar suas entradas.

## Recursos
- Funcionalidade de host de teclado USB
- Compatível com microcontroladores ESP32-S2
- Design modular com componentes separados para fácil personalização

## Primeiros Passos

### Pré-requisitos
- Microcontrolador ESP32-S2
- Teclado USB

### Instalação
1. Instale o PlatformIO IDE ou integre-o ao Visual Studio Code.
2. Clone o repositório para sua máquina local.
3. Abra o projeto usando o PlatformIO ou o ambiente de desenvolvimento ESP32 de sua preferência.

### Uso
- Conecte seu teclado USB ao microcontrolador ESP32-S2.
- Ligue o ESP32-S2.
- O dispositivo começará a processar as entradas do teclado.

## Estrutura
- `.gitignore`: Arquivo de ignore do Git.
- `.pio`: Contém saídas e configurações de build do PlatformIO.
- `.vscode`: Arquivos de configuração do VSCode para o projeto.
- `include`: Contém arquivos de inclusão para o projeto.
- `lib`: Bibliotecas usadas no projeto.
  - `RgbPixel`: Biblioteca para controle de pixels RGB.
  - `USBHostHid`: Biblioteca para manipulação de HID Host USB.
  - `USBKeyboard`: Biblioteca para funcionalidades de teclado USB.
- `platformio.ini`: Arquivo de configuração do PlatformIO.
- `src`: Contém o código fonte principal do projeto.
- `test`: Contém scripts de teste para o projeto.

## Bibliotecas
- RgbPixel: Gerencia o controle de pixels RGB.
- USBHostHid: Gerencia funcionalidades de HID Host USB.
- USBKeyboard: Implementa a interação com teclado USB.

## Contribuindo
Contribuições para este projeto são bem-vindas. Por favor, siga as práticas padrão de codificação e forneça descrições detalhadas de suas contribuições.

## Licença
Este projeto está licenciado sob [LICENSE](LICENSE). Veja o arquivo [LICENSE](LICENSE) para mais detalhes.
