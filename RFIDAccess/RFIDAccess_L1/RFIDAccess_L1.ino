/*
 * Um botão para registro de um cartão.
 * O Arduino comecerá no modo de acesso, onde cartões serão dados acesso, ou não.
 *
 * Ao pressionar o botão, o arduino entrará no modo de registro,
 * onde o próximo cartão lido será gravado como cartão válido.
 *
 * Depois, sairá do modo de registro e entrará no modo de acesso,
 * onde o cartão recém gravado deverá ter acesso concebido.
 *
 * ------------------------
 * MFRC522      Arduino
 * Reader/PCD   Uno/101
 * Pin          Pin
 * ------------------------
 * RST          9
 * SDA(SS)      10
 * MOSI         11 / ICSP-4
 * MISO         12 / ICSP-1
 * SCK          13 / ICSP-3
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

#define MASTER_BTN_PIN 2 // Pino do botão de registro

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria instância MFRC522

byte masterCard[4];
byte readCard[4];

bool isSettingMasterCard;
bool isLookingForAccess;

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();

  isSettingMasterCard = false;
  isLookingForAccess = true;

  pinMode(MASTER_BTN_PIN, INPUT_PULLUP);

  Serial.println(F("Setup concluído"));
}

void loop()
{
  if (isLookingForAccess)
  {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
      // read new card
      for (uint8_t i = 0; i < 4; i++)
      {
        readCard[i] = mfrc522.uid.uidByte[i];
        // Serial.print(readCard[i], HEX);
      }

      bool isUser = isMasterCard(readCard);

      if (isUser)
      {
        Serial.println(F("SOPA TÀ NA MÃO! :D"));
        delay(2000);
      }
      else
      {
        Serial.println(F("SEM SOPA PRA VC :/"));
        delay(2000);
      }

    }
  }

  // Tentando ler novo cartão master
  if (isSettingMasterCard)
  {
    isLookingForAccess = false;

    if (!mfrc522.PICC_IsNewCardPresent()) { return; }
    if (!mfrc522.PICC_ReadCardSerial()) { return; }

    // Grava o uid do cartão master
    for (uint8_t i = 0; i < 4; i++)
    {
      masterCard[i] = mfrc522.uid.uidByte[i];
    }

    Serial.println(F("Novo cartão master gravado"));

    delay(2000);
    isSettingMasterCard = false;
    isLookingForAccess = true;
  }

  if (digitalRead(MASTER_BTN_PIN) == LOW)
  {
    Serial.println(F("Aproxime um novo cartão master."));
    isSettingMasterCard = true;
  }
}

bool isMasterCard(byte card[])
{
  for (uint8_t i = 0; i < 4; i++)
  {
    if (card[i] != masterCard[i])
    {
      // retorna falha assim que achar um byte errado
      return false;
    }
  }

  return true;
}