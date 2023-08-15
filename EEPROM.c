/*
 *      Armazenamento Externo EEPROM - EEPROM.c
 *
 *      Data: 12 de agosto, 2023
 *      Autor: Gabriel Luiz
 *      Contato: (31) 97136-4334 || gabrielluiz.eletro@gmail.com
 */
/*
 * 		- Links Úteis -
 *
 */

/* Private Includes ----------------------------------------------------------*/
/* USER CODE BEGIN PI */

#include "EEPROM.h"

/* USER CODE END PI */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

extern I2C_HandleTypeDef hi2c1;

/* USER CODE END EV */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define EEPROM_HANDLE_I2C &hi2c1

/* USER CODE END PD */

/* External functions ------------------------------------------------------------*/
/* USER CODE BEGIN EF */
/* USER CODE END EF */

/* Private variables --------------------------------------------------------*/
/* USER CODE BEGIN PV */

uint16_t ENDERECO = 0xA0;
uint16_t PAGINAS = 512;
uint16_t TAMANHO_PAGINA = 128;
uint8_t DESLOCAMENTO = 7;

/* USER CODE END PV */

/* Private functions ------------------------------------------------------------*/
/* USER CODE BEGIN PF */

/*----------------------- BYTES PARA LER OU ESCREVER ----------------------*/

/**
 * @brief Função usada para determinar a quantidade de bytes a escrever ou ler
 * @param Tamanho: Quantidade de bytes
 * @param Posicao: Posição de escrita na página
 * @param tamanho_pagina: Tamanho de uma unica página
 * @retval Quantidade de bytes para se escrever na página
 */

uint16_t bytesToWriteRead(uint16_t Tamanho, uint16_t Posicao,
		uint16_t tamanho_pagina) {
	if ((Tamanho + Posicao) < tamanho_pagina)
		return Tamanho;
	else
		return tamanho_pagina - Posicao;
}

/*------------------------- INICIALIZAR A EEPROM --------------------------*/

/**
 * @brief Função utilizada para inicializar corretamente a biblioteca
 * @param Paginas_d: Quantidade total de páginas
 * @param Tamanho_pagina_d:  Tamanho total de cada página
 * @retval ***NONE***
 */

void EEPROM_Init(uint16_t Paginas_d, uint16_t Tamanho_pagina_d) {
	PAGINAS = Paginas_d;
	TAMANHO_PAGINA = Tamanho_pagina_d;
	for (int i = TAMANHO_PAGINA; i > 1; DESLOCAMENTO++)
		i = i / 2;
}

/*------------------------ ESCREVER DADOS NA EEPROM ----------------------*/

/**
 * @brief Função utilizada para escrita na memória EEPROM
 * @param pagina: Página a ser escrita
 * @param posicao: Posição a ser escrito
 * @param valor: Valor a ser escrito na memória
 * @retval Confirmação do valor escrito
 */

uint8_t EEPROM_Write(uint16_t pagina, uint16_t posicao, uint8_t valor) {
	uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;

	HAL_I2C_Mem_Write(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS, 2,
			&valor, 1, 1000);

	HAL_Delay(5);
	return EEPROM_Read(pagina, posicao);
}

/*------------------- ESCREVER MULTIPLOS DADOS NA EEPROM ------------------*/

/**
 * @brief Função usada para escrita de uma sequência de dados na EEPROM
 * @param pagina: Página a ser escrita
 * @param posicao: Posição a ser escrito
 * @param pDados: Ponteiro para os dados a serem escritos
 * @param size: Quantidade de bytes a serem escritos
 * @retval Status final da escrita na memória
 */

HAL_StatusTypeDef EEPROM_Write_MultiData(uint16_t pagina, uint16_t posicao,
		uint8_t *pDados, uint16_t size) {
	HAL_StatusTypeDef STATUS = HAL_OK;
	uint16_t NUMERO_DE_PAGINAS = (pagina + posicao) / 128 + 1;
	uint16_t POSICAO = 0;
	for (int i = 0; i < NUMERO_DE_PAGINAS; i++) {

		uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;
		uint16_t BYTES_TO_WRITE = bytesToWriteRead(size, posicao,
				TAMANHO_PAGINA);

		if (HAL_I2C_Mem_Write(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS,
				2, pDados, BYTES_TO_WRITE, 1000) != HAL_OK)
			STATUS = HAL_ERROR;
		pagina += 1;
		posicao = 0;
		size = size - BYTES_TO_WRITE;
		POSICAO += BYTES_TO_WRITE;
		HAL_Delay(5);
	}
	return STATUS;
}

/*--------------------------- LER DADOS NA EEPROM -------------------------*/

/**
 * @brief Função utilizada para ler dados na memória EEPROM
 * @param pagina: Página a ser lida
 * @param posicao: Posição a ser lido
 * @retval Valor lido na memória
 */

uint8_t EEPROM_Read(uint16_t pagina, uint16_t posicao) {
	uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;
	uint8_t EEPROM_MEMORY_VALUE = 0;
	HAL_I2C_Mem_Read(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS, 2,
			&EEPROM_MEMORY_VALUE, 1, 1000);

	return EEPROM_MEMORY_VALUE;
}

/*---------------------- LER MULTIPLOS DADOS NA EEPROM --------------------*/

/**
 * @brief Função usada para escrita de uma sequência de dados na EEPROM
 * @param pagina: Página a ser lida
 * @param posicao: Posição a ser lido
 * @param pDados: Ponteiro para os dados a serem lidos
 * @param size: Quantidade de bytes a serem lidos
 * @retval Status final da leitura na memória
 */

HAL_StatusTypeDef EEPROM_Read_MultiData(uint16_t pagina, uint16_t posicao,
		uint8_t *pDados, uint16_t size) {
	HAL_StatusTypeDef STATUS = HAL_OK;
	uint16_t NUMERO_DE_PAGINAS = (pagina + posicao) / 128 + 1;
	uint16_t POSICAO = 0;
	for (int i = 0; i < NUMERO_DE_PAGINAS; i++) {

		uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;
		uint16_t BYTES_TO_WRITE = bytesToWriteRead(size, posicao,
				TAMANHO_PAGINA);

		if (HAL_I2C_Mem_Read(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS,
				2, pDados, BYTES_TO_WRITE, 1000) != HAL_OK)
			STATUS = HAL_ERROR;
		pagina += 1;
		posicao = 0;
		size = size - BYTES_TO_WRITE;
		POSICAO += BYTES_TO_WRITE;
		HAL_Delay(5);
	}
	return STATUS;
}

/*---------------------- CHECAR ENDEREÇO DE COMUNICAÇÃO -------------------*/

/**
 * @brief Função utilizada para determinar o endereço de comunicação com a EEPROM
 * @param ***NONE***
 * @retval Endereço de comunicação
 */

uint16_t checkAdress() {
	for (int i = 0; i < 256; i++) {
		if (HAL_I2C_IsDeviceReady(&hi2c1, i, 5, 100) == HAL_OK) {
			ENDERECO = i;
			return ENDERECO;
		}
	}
	return 0x00;
}
