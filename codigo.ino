#include <WiFi.h>
#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
const char* ssid = "*********";
const char* password = "*********";

WiFiServer server(80);

const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">

<head>

  <meta charset="UTF-8">

  <meta
    name="viewport"
    content="width=device-width, initial-scale=1.0"
  >

  <title>Estação Meteorológica</title>

  <style>

    body {
      font-family: Arial, sans-serif;
      background: #eef2f5;
      margin: 0;
      padding: 20px;
      text-align: center;
    }

    .painel {
      max-width: 700px;
      margin: auto;
      background: white;
      padding: 20px;
      border-radius: 16px;
      box-shadow: 0 4px 15px rgba(0, 0, 0, 0.15);
    }

    h1 {
      margin-top: 5px;
    }

    .leituras {
      display: flex;
      gap: 15px;
      justify-content: center;
      flex-wrap: wrap;
      margin-bottom: 20px;
    }

    .cartao {
      flex: 1;
      min-width: 180px;
      padding: 18px;
      background: #f3f5f7;
      border-radius: 12px;
    }

    .titulo {
      font-size: 18px;
    }

    .valor {
      font-size: 32px;
      font-weight: bold;
      margin-top: 8px;
    }

    .grafico {
      position: relative;
      width: 100%;
      height: 330px;
    }

    canvas {
      width: 100%;
      height: 100%;
      border: 1px solid #dddddd;
      border-radius: 10px;
      background: white;
    }

    .legenda {
      margin-top: 12px;
      font-size: 15px;
    }

    .temperatura {
      color: #d33;
      font-weight: bold;
    }

    .umidade {
      color: #1685c5;
      font-weight: bold;
    }

    .status {
      margin-top: 15px;
      color: #555;
    }

  </style>

</head>

<body>

  <div class="painel">

    <h1>Estação Meteorológica</h1>

    <div class="leituras">

      <div class="cartao">
        <div class="titulo">Temperatura</div>

        <div class="valor">
          <span id="temperatura">--</span> °C
        </div>
      </div>

      <div class="cartao">
        <div class="titulo">Umidade</div>

        <div class="valor">
          <span id="umidade">--</span> %
        </div>
      </div>

    </div>

    <h2>Gráfico em tempo real</h2>

    <div class="grafico">
      <canvas id="grafico"></canvas>
    </div>

    <div class="legenda">
      <span class="temperatura">Temperatura</span>
      &nbsp;&nbsp;|&nbsp;&nbsp;
      <span class="umidade">Umidade</span>
    </div>

    <div class="status" id="status">
      Aguardando a primeira medição...
    </div>

  </div>

<script>

  const canvas = document.getElementById("grafico");
  const contexto = canvas.getContext("2d");

  let temperaturas = [];
  let umidades = [];
  let horarios = [];

  const quantidadeMaxima = 30;

  function ajustarCanvas() {

    const proporcao = window.devicePixelRatio || 1;

    canvas.width = canvas.clientWidth * proporcao;
    canvas.height = canvas.clientHeight * proporcao;

    contexto.setTransform(
      proporcao,
      0,
      0,
      proporcao,
      0,
      0
    );
  }

  function desenharGrafico() {

    const largura = canvas.clientWidth;
    const altura = canvas.clientHeight;

    const margemEsquerda = 45;
    const margemDireita = 20;
    const margemSuperior = 20;
    const margemInferior = 35;

    const larguraUtil =
      largura - margemEsquerda - margemDireita;

    const alturaUtil =
      altura - margemSuperior - margemInferior;

    contexto.clearRect(
      0,
      0,
      largura,
      altura
    );

    contexto.fillStyle = "#ffffff";

    contexto.fillRect(
      0,
      0,
      largura,
      altura
    );

    contexto.font = "12px Arial";
    contexto.fillStyle = "#555555";
    contexto.strokeStyle = "#dddddd";
    contexto.lineWidth = 1;

    for (let valor = 0; valor <= 80; valor += 20) {

      const y =
        margemSuperior +
        alturaUtil -
        (valor / 80) * alturaUtil;

      contexto.beginPath();

      contexto.moveTo(
        margemEsquerda,
        y
      );

      contexto.lineTo(
        largura - margemDireita,
        y
      );

      contexto.stroke();

      contexto.fillText(
        valor.toString(),
        8,
        y + 4
      );
    }

    contexto.strokeStyle = "#333333";

    contexto.beginPath();

    contexto.moveTo(
      margemEsquerda,
      margemSuperior
    );

    contexto.lineTo(
      margemEsquerda,
      altura - margemInferior
    );

    contexto.stroke();

    contexto.beginPath();

    contexto.moveTo(
      margemEsquerda,
      altura - margemInferior
    );

    contexto.lineTo(
      largura - margemDireita,
      altura - margemInferior
    );

    contexto.stroke();

    if (temperaturas.length < 2) {
      return;
    }

    const intervaloX =
      larguraUtil / (quantidadeMaxima - 1);

    contexto.strokeStyle = "#d33";
    contexto.lineWidth = 3;
    contexto.beginPath();

    temperaturas.forEach((valor, indice) => {

      const x =
        margemEsquerda +
        indice * intervaloX;

      const y =
        margemSuperior +
        alturaUtil -
        (valor / 100) * alturaUtil;

      if (indice === 0) {
        contexto.moveTo(x, y);
      } else {
        contexto.lineTo(x, y);
      }
    });

    contexto.stroke();

    contexto.strokeStyle = "#1685c5";
    contexto.lineWidth = 3;
    contexto.beginPath();

    umidades.forEach((valor, indice) => {

      const x =
        margemEsquerda +
        indice * intervaloX;

      const y =
        margemSuperior +
        alturaUtil -
        (valor / 100) * alturaUtil;

      if (indice === 0) {
        contexto.moveTo(x, y);
      } else {
        contexto.lineTo(x, y);
      }
    });

    contexto.stroke();

    contexto.fillStyle = "#555555";
    contexto.font = "11px Arial";

    horarios.forEach((horario, indice) => {

      if (
        indice === 0 ||
        indice === horarios.length - 1 ||
        indice % 5 === 0
      ) {

        const x =
          margemEsquerda +
          indice * intervaloX;

        contexto.fillText(
          horario,
          x - 16,
          altura - 10
        );
      }
    });
  }

  async function atualizarDados() {

    try {

      const resposta = await fetch(
        "/dados?tempo=" + Date.now()
      );

      const dados = await resposta.json();

      if (dados.erro) {

        document.getElementById("status").innerText =
          "Erro ao ler o DHT11.";

        return;
      }

      document.getElementById("temperatura").innerText =
        dados.temperatura.toFixed(1);

      document.getElementById("umidade").innerText =
        dados.umidade.toFixed(1);

      const agora = new Date();

      const horario =
        agora.getHours().toString().padStart(2, "0") +
        ":" +
        agora.getMinutes().toString().padStart(2, "0") +
        ":" +
        agora.getSeconds().toString().padStart(2, "0");

      temperaturas.push(dados.temperatura);
      umidades.push(dados.umidade);
      horarios.push(horario);

      if (temperaturas.length > quantidadeMaxima) {
        temperaturas.shift();
        umidades.shift();
        horarios.shift();
      }

      document.getElementById("status").innerText =
        "Última atualização: " + horario;

      desenharGrafico();

    } catch (erro) {

      document.getElementById("status").innerText =
        "Falha na comunicação com o ESP32.";
    }
  }

  window.addEventListener("resize", function() {
    ajustarCanvas();
    desenharGrafico();
  });

  ajustarCanvas();
  atualizarDados();

  setInterval(atualizarDados, 2000);

</script>

</body>
</html>
)rawliteral";

void setup() {

  Serial.begin(115200);
  delay(1000);

  dht.begin();

  Serial.println();
  Serial.println("Conectando ao Wi-Fi...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi conectado!");

  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  server.begin();

  Serial.println("Servidor iniciado!");
}

void loop() {

  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  String requisicao = "";
  unsigned long tempoInicial = millis();

  while (
    client.connected() &&
    millis() - tempoInicial < 2000
  ) {

    if (client.available()) {

      char caractere = client.read();
      requisicao += caractere;

      if (requisicao.endsWith("\r\n\r\n")) {
        break;
      }
    }
  }

  if (requisicao.indexOf("GET /dados") >= 0) {

    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    client.println("HTTP/1.1 200 OK");

    client.println(
      "Content-Type: application/json; charset=UTF-8"
    );

    client.println("Cache-Control: no-cache");
    client.println("Connection: close");
    client.println();

    if (
      isnan(temperatura) ||
      isnan(umidade)
    ) {

      client.println(
        "{\"erro\":true}"
      );

      Serial.println("Erro ao ler o DHT11.");

    } else {

      client.print("{\"erro\":false,");

      client.print("\"temperatura\":");
      client.print(temperatura, 1);

      client.print(",\"umidade\":");
      client.print(umidade, 1);

      client.println("}");

      Serial.print("Temperatura: ");
      Serial.print(temperatura, 1);
      Serial.print(" °C | Umidade: ");
      Serial.print(umidade, 1);
      Serial.println(" %");
    }
  }

  else if (
    requisicao.indexOf("GET /favicon.ico") >= 0
  ) {

    client.println("HTTP/1.1 204 No Content");
    client.println("Connection: close");
    client.println();
  }

  else {

    client.println("HTTP/1.1 200 OK");

    client.println(
      "Content-Type: text/html; charset=UTF-8"
    );

    client.println("Connection: close");
    client.println();

    client.print(paginaHTML);

    Serial.println("Página principal enviada.");
  }

  delay(5);
  client.stop();
}