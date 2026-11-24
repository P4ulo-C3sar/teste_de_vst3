# VocalClarity — VST3 para limpeza vocal

Projeto JUCE/CMake para Windows x64. Inclui controles de Noise Learn, redução de ruído, Hiss, Hum, Clicks, Sibilance, De-reverb e Mix. O DSP é um protótipo clássico de baixa latência, sem modelo neural; use os controles com moderação e escute artefatos.

## Compilar sem instalar C++ no computador

O workflow `.github/workflows/build-vst3.yml` compila no runner Windows do GitHub Actions e publica `VocalClarity-Windows-x64-VST3.zip`. O projeto está organizado em arquivos e pastas normais do repositório; não é preciso enviar um ZIP manualmente.

1. Envie `CMakeLists.txt`, `Source/`, `.github/workflows/build-vst3.yml`, `demo.html` e este README ao repositório `teste_de_vst3`.
2. Abra **Actions** no repositório e habilite workflows, se solicitado. O commit do código dispara a compilação.
3. Quando terminar, abra a execução verde, baixe o artefato `VocalClarity-Windows-x64-VST3` e extraia o ZIP.
4. Copie `VocalClarity.vst3` para `C:\Program Files\Common Files\VST3`; no FL Studio, atualize o plugin database/rescan.

O artefato fica disponível por 30 dias neste workflow. O build remoto é gratuito dentro das cotas de GitHub Actions disponíveis à sua conta/repositório; é necessário ter uma conta e um repositório GitHub. Não envie gravações vocais ao repositório: o workflow compila apenas o código-fonte.

## Faust IDE ou JUCE?

[Faust IDE](https://faustide.grame.fr/) é uma opção online real para experimentar DSP, mas a documentação oficial descreve sua compilação para arquiteturas suportadas e projetos Faust; saída WAM é para navegador e não é VST3 nativo do FL Studio. Para obter o formato pedido, este projeto usa **JUCE + CMake + GitHub Actions**.

## Controles e operação

- **Noise Learn**: ligue durante um trecho sem voz que contenha apenas ruído; desligue ao entrar a voz. Perfil simples adaptativo, guardado no estado do projeto.
- **Noise Reduction**: redução espectral estimada por banda.
- **Hiss**: reforço da redução de altas frequências.
- **Hum**: atenuação de harmônicos estimados de 50/60 Hz. O protótipo não estima automaticamente a rede elétrica local.
- **Clicks**: suavização de transientes impulsivos.
- **Sibilance**: atenuação conservadora de altas frequências; não é um de-esser detector de fonemas completo.
- **De-reverb**: redução experimental de cauda reverberante. Não desfaz ecos fortes nem substitui diálogo neural/de-reverb offline.
- **Mix**: mistura paralela do sinal original e tratado.

## Análise comparativa de referências (REA)

- **ReaFIR/Subtract**: seu manual recomenda construir o perfil num trecho contendo apenas o ruído e então congelá-lo; adotamos esse fluxo de Learn.
- **iZotope RX Dialogue De-noise**: usa limiares por banda, modos manual/adaptativo e Learn. Isso inspira parâmetros dedicados e redução moderada para preservar voz.
- **iZotope RX De-reverb**: é um módulo distinto do denoiser. Por isso, De-reverb permanece separado e está marcado experimental.
- **FabFilter Pro-DS**: expõe controles vocais e parâmetros automatizáveis; inspirou a apresentação simples de Sibilance/Mix. O nosso controle é menos sofisticado.
- **Steinberg VST3/JUCE**: parâmetros, estado e processamento em blocos seguem o modelo de plugin; compatibilidade final depende de validar o binário no host FL Studio.

Fontes primárias: [manual ReaEffects/ReaFIR](https://www.reaper.fm/guides/ReaEffectsGuide.pdf), [iZotope Dialogue De-noise](https://help.izotope.com/docs/rx-plug-in-pack/Dialogue_De-noise.html), [módulos RX disponíveis](https://support.izotope.com/hc/en-us/articles/47669810062093-Where-to-find-and-use-individual-RX-modules), [JUCE CMake API](https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md), [documentação Faust IDE](https://faustdoc.grame.fr/manual/overview/), [API Steinberg VST3](https://steinbergmedia.github.io/vst3_dev_portal/pages/Technical%2BDocumentation/API%2BDocumentation/Index.html).

## Licenciamento e limitações

JUCE é dual-licensed sob a licença JUCE e AGPLv3. Antes de distribuir binários fechados, confira a [licença e FAQ oficial](https://juce.com/get-juce/) e os termos vigentes; a compilação técnica não concede automaticamente direito a distribuição proprietária. O workflow fixa JUCE 8.0.12 para build repetível.

Este ambiente não possui toolchain C++; portanto o plugin não foi compilado ou validado no FL Studio. O DSP não é comparável em qualidade a RX ou restauração neural e ainda precisa de avaliação auditiva com material de teste.
