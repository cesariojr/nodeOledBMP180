# nodeOledBMP180

Esse protótipo utiliza o sensor Bosch BMP180 (https://cdn-shop.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf) para obter as grandezas de temperatura, pressão atmosférica e altitude, para depois conectar no IBM Watson IoT Platform utilizando um microcontrolador NodeMCU como um device registrado.

Todo código foi feito através da IDE do Arduino, personalizada para compatibilidade com o NodeMCU.

Foi adicionado um display OLED (https://www.filipeflop.com/produto/display-oled-0-96-i2c-azul-amarelo/) e a biblioteca esp8266-oled-ssd1306 (https://github.com/squix78/esp8266-oled-ssd1306) para controlar o display.

Foi adicionada a fonte Courier 10 usando o gerador de fontes da biblioteca OLED (http://oleddisplay.squix.ch/#/home) e o código adicional foi copiado dentro do arquivo OLEDDisplayFonts.h que fica dentro da pasta da biblioteca OLED.

A conexão do sensor BMP180 e o display OLED como o NodeMCU foi feita através do protocolo I2C.
![alt text](https://github.com/cesariojr/nodeOledBMP180/blob/master/node.JPG)

Os dados derivados da medição do sensor são formatados em JSON e enviados para o IBM Watson IoT Platform, utilizando o protocolo MQTT. Um dashboard simples foi criado para visualização em tempo real:

![alt text](https://github.com/cesariojr/nodeOledBMP180/blob/master/dashboard.JPG)
