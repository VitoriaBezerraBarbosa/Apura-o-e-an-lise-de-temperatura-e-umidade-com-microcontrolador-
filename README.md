# Apuração e análise de temperatura e umidade com microcontrolador

Estação Meteorológica com ESP32

Projeto de uma estação meteorológica usando ESP32 e sensor DHT11.

O ESP32 coleta os dados do sensor DHT11 que mede a temperatura e umidade, e disponibiliza os dados em uma página web
acessível pelo celular conectado à mesma rede Wi-Fi.

## Componentes

- ESP32
- Sensor DHT11
- Resistor de 10 kΩ
- Protoboard
- Jumpers

## Ligações

- VCC do DHT11 → 3,3 V
- GND do DHT11 → GND
- DATA do DHT11 → GPIO 4

## Bibliotecas

- WiFi
- DHT sensor library
