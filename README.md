# VoxClarity — VST3 de redução de ruído vocal

Plugin JUCE para Windows x64 que usa RNNoise em tempo real para reduzir ruído de fundo em voz. A interface é original, com cores neon, controle central visual e knobs para Strength, Mix, Voice Gate, Output e monitoramento Diff, inspirada nas imagens fornecidas sem copiar logotipo ou identidade da Waves.

## Estado

O projeto fonte está pronto para build pelo GitHub Actions em `.github/workflows/build-vst3.yml`. Este ambiente não tem compilador C++/CMake e não há checkout autorizado do repositório GitHub aqui; portanto nenhum binário foi produzido nem foi possível testar dentro do FL Studio. Após enviar estes arquivos ao seu repositório, a execução do workflow publicará o artefato Windows x64 VST3. Instale a pasta `VoxClarity.vst3` em `C:\Program Files\Common Files\VST3` e faça rescan no FL Studio.

## DSP e controles

RNNoise processa blocos de 480 amostras a 48 kHz. Esta versão exige que o projeto do FL Studio esteja em 48 kHz; ela silencia a saída em outras taxas porque ainda não inclui conversor de amostragem. Cada canal usa estado independente. O plugin informa latência de 480 amostras. Entrada mono e estéreo são aceitas; estéreo usa detecção no sinal mono e mantém o sinal espacial pela combinação do canal processado com o dry.

- **Strength** — mistura entre sinal original e RNNoise.
- **Mix** — paralelo entre dry e processado.
- **Voice Gate** — atenuação controlada pela probabilidade de voz.
- **Output** — ganho de saída em dB.
- **Diff** — monitora o resíduo removido para avaliar artefatos.

RNNoise pode afetar timbre, consoantes ou vozes mais fracas, dependendo da gravação. Ajuste Strength ouvindo em contexto. O Diff não deve ser usado como saída final.

## Build no GitHub

1. Envie a pasta do projeto ao repositório. O projeto é GPL-3.0-or-later; mantenha a licença e os avisos de terceiros no repositório e ao redistribuir versões modificadas.
2. Em **Actions**, execute **Build VoxClarity VST3** ou faça push de alterações nos arquivos de build.
3. Baixe o artefato `VoxClarity-Windows-x64-VST3` da execução concluída.
4. Extraia `VoxClarity.vst3` para a pasta VST3 comum e atualize a lista de plugins do FL Studio.

O build usa JUCE 8.0.12 e RNNoise do repositório indicado, buscados durante a configuração do CMake. GitHub Actions precisa de rede para buscar essas dependências. Compatibilidade final deve ser confirmada carregando o artefato no FL Studio 2026.

## Licença

Este projeto é distribuído sob GNU GPL versão 3 ou posterior por incorporar RNNoise sob GPL-3.0. Consulte `LICENSE` e `THIRD_PARTY_NOTICES.md`. JUCE usa termos GPL/comerciais; a redistribuição segue GPL neste projeto. Esta distribuição não usa nem inclui código, modelos, marcas ou presets da Waves.

