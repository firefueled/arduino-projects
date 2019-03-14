/*
 * Um botão para registro de cartões.
 * O Arduino comecerá no modo de acesso, onde cartões serão dados acesso, ou não.
 *
 * Ao pressionar o botão, o arduino entrará no modo de registro,
 * onde o próximo cartão lido será gravado como um dos cartções válidos.
 *
 * Depois, sairá do modo de registro e entrará no modo de acesso,
 * onde qualquer um dos cartões gravado previamente deverá ter acesso concebido.
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

#define ADD_USER_BTN_PIN 2 // Pino do botão de registro

#define MAX_USER_CARDS 10 // Quantidade máxima de cartões registrados

MFRC522 mfrc522(SS_PIN, RST_PIN); // Cria instância MFRC522

byte userCards[MAX_USER_CARDS][4];
byte readCard[4];

int userCardsCount = 0;

bool isAddingNewUserCard;
bool isLookingForAccess;

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();

  isAddingNewUserCard = false;
  isLookingForAccess = true;

  pinMode(ADD_USER_BTN_PIN, INPUT_PULLUP);

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

      bool isUser = isUserCard(readCard);

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

  // Tentando ler novo cartão de usuário
  if (isAddingNewUserCard)
  {

    isLookingForAccess = false;

    if (!mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    if (!mfrc522.PICC_ReadCardSerial())
    {
      return;
    }

    // grava o uid do novo cartão de usuário
    for (uint8_t i = 0; i < 4; i++)
    {
      userCards[userCardsCount][i] = mfrc522.uid.uidByte[i];
    }
    userCardsCount++;

    Serial.println(F("Novo cartão de usuário gravado."));

    delay(2000);
    isLookingForAccess = true;
    isAddingNewUserCard = false;
  }

  if (digitalRead(ADD_USER_BTN_PIN) == LOW)
  {
    // nada a fazer se o número máximo de cartões foi atingido
    if (userCardsCount < MAX_USER_CARDS)
    {
      Serial.println(F("Aproxime um novo cartão de usuário."));
      isAddingNewUserCard = true;
    }
    else
    {
      Serial.println(F("Sem espaço para mais cartões :("));
      delay(2000);
    }
  }
}

bool isUserCard(byte card[])
{
  // loop por todos os possíveis cartões de usuário
  for (uint8_t i = 0; i < MAX_USER_CARDS; i++)
  {
    bool didCardFail = false;
    // compara um dos cartões de usuário ao cartão lido
    for (uint8_t j = 0; j < 4; j++)
    {
      if (card[j] != userCards[i][j])
      {
        // retorna falha assim que achar um byte errado
        didCardFail = true;
        break;
      }
    }

    // retorna sucesso assim que achar um cartão válido
    if (!didCardFail)
    {
      return true;
    }
  }

  return false;
}
