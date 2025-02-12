# RC Master Mind

Leonor Francisco, 
Rita Melo

## Descrição
Este projeto implementa uma versão simplificada do jogo Master Mind usando a interface de sockets. Consiste em:

Game Server (GS): Um servidor responsável por gerir os jogos e responder às interações dos jogadores.
Player Application (Player): Uma aplicação cliente que permite aos jogadores interagir com o servidor, realizar jogadas e consultar informações.
O projeto utiliza protocolos de comunicação baseados no paradigma cliente-servidor, com suporte para UDP e TCP.

## Funcionalidades
### Game Server (GS)
Gere e valida partidas de Master Mind.
Responde a tentativas de adivinhação com feedback sobre a posição e cor.
Gere solicitações de listagem de tentativas e placares.
### Player Application (Player)
Inicia novas partidas, enviando identificação do jogador (PLID) e tempo máximo de jogo.
Realiza tentativas de adivinhar a chave secreta.
Consulta o histórico de tentativas e o placar.
Opera no modo de depuração (debug), especificando a chave secreta.
Finaliza ou sai do jogo a qualquer momento.

## Uso
### Compilação
Use o comando make para compilar o projeto. Os executáveis serão gerados no diretório atual.
### Execução
### Game Server (GS)
./GS [-p GSport] [-v]
GSport: Porta para aceitar conexões (padrão: 58000 + GN).
-v: Ativa o modo verbose.
### Player Application (Player)
./player [-n GSIP] [-p GSport]
GSIP: Endereço IP do Game Server (padrão: localhost).
GSport: Porta usada pelo servidor (padrão: 58000 + GN).

## Protocolo de Comunicação
### UDP (Jogo)
start: Inicia um novo jogo.
try: Envia uma tentativa de adivinhação.
quit: Finaliza a partida em andamento.
### TCP (Listagem e Placar)
show_trials: Solicita o histórico de tentativas.
scoreboard: Solicita o placar dos 10 melhores jogadores.

## Requisitos
Linguagem: C/C++
Sistema operacional: Unix/Linux
Chamadas de sistema utilizadas:
UDP: socket(), sendto(), recvfrom(), select(), bind()
TCP: socket(), connect(), write(), read(), select(), accept(), listen(), bind()
