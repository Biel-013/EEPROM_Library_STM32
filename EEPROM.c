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

extern I2C_HandleTypeDef hi2c1; /* Variável externa de configuração do I2C */

/* USER CODE END EV */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define EEPROM_HANDLE_I2C &hi2c1 /* Ponteiro para o handle de configuração I2C */

/* USER CODE END PD */

/* External functions ------------------------------------------------------------*/
/* USER CODE BEGIN EF */
/* USER CODE END EF */

/* Private variables --------------------------------------------------------*/
/* USER CODE BEGIN PV */

uint16_t ENDERECO = 0xA0; /* Endereço de slave I2C da memoria EEPROM */
uint16_t PAGINAS = 512; /* Quantidade total de páginas da memoria EEPROM */
uint16_t TAMANHO_PAGINA = 128; /* Tamanho total de cada página individual */
uint8_t DESLOCAMENTO = 7; /* Deslocamento necessário para acesso aos endereços
 de memória, é um logaritimo de base 2 do tamanho das páginas */

/* USER CODE END PV */

/* Private functions ------------------------------------------------------------*/
/* USER CODE BEGIN PF */

/*----------------------- BYTES PARA LER OU ESCREVER ----------------------*/

/**
 * @brief Função usada para determinar a quantidade de bytes a escrever ou ler
 * @param Tamanho: Quantidade de bytes
 * @param Posicao: Posição de escrita na página
 * @retval Quantidade de bytes para se escrever na página
 */

uint16_t bytesToWriteRead(uint16_t Tamanho, uint16_t Posicao) {
	/* Caso a soma do tamanho de bytes a escrever (Tamanho) mais a posição
	 * atual na página (Posicao) não supere o tamanho total de uma página,
	 * a quantidade de bytes a escrever será igual a do tamanho de bytes */
	if ((Tamanho + Posicao) < TAMANHO_PAGINA)
		return Tamanho;
	else
		/* Caso contrário, a quantidade a escrever, será o que sobrou de
		 * espaço a frente da posição atual na página*/
		return TAMANHO_PAGINA - Posicao;
}

/*------------------------- INICIALIZAR A EEPROM --------------------------*/

/**
 * @brief Função utilizada para inicializar corretamente a biblioteca
 * @param Paginas_d: Quantidade total de páginas
 * @param Tamanho_pagina_d:  Tamanho total de cada página
 * @retval ***NONE***
 */

void EEPROM_Init(uint16_t Paginas_d, uint16_t Tamanho_pagina_d) {
	PAGINAS = Paginas_d; /* Atualiza a quantidade de páginas */

	TAMANHO_PAGINA = Tamanho_pagina_d; /* Atualiza a quantidade de páginas */

	/* Realiza um cálculo para determinar o novo deslocamento necessário,
	 * se assemelha ao funcionamento de uma função logarítima */
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
	/* Determina o endereço de memória para acesso na EEPROM. Os
	 * bits deslocados são a página selecionada, e os bits entre o
	 * deslocamento são a posição na página */
	uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;

	/* Escreve um único valor de 1 byte na região de memória selecionada */
	HAL_I2C_Mem_Write(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS, 2,
			&valor, 1, 1000);

	HAL_Delay(5); /* Delay necessário para processo de escrita na memória */

	return EEPROM_Read(pagina, posicao); /* retorna o valor lido na mesma
	 posição de memória, dee ser o mesmo que o escrito */
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

	HAL_StatusTypeDef STATUS = HAL_OK; /* Variável de verificação de
	 sucesso na escrita */

	uint16_t ULTIMA_PAGINA = (pagina + posicao) / 128 + 1; /* Última pagina
	 acessada na leitura ou escrita */

	/* Laço para escrita sequêncial na EEPROM */
	for (int i = 0; i < ULTIMA_PAGINA; i++) {

		/* Endereço de acesso na memória para escrita */
		uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;

		/* Quantidade de bytes a se escrever na página || máximo 128 */
		uint16_t BYTES_TO_WRITE = bytesToWriteRead(size, posicao);

		/* Função de envio de dados por I2C, junto a uma verificação de ERRO */
		if (HAL_I2C_Mem_Write(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS,
				2, pDados, BYTES_TO_WRITE, 1000) != HAL_OK)
			STATUS = HAL_ERROR; /* Caso algo dê errado, STATUS recebe  HAL_ERROR */

		pagina += 1; /* Incrementa 1 página, pois a função "bytesToWriteRead"
		 garante que o espaço restante total seja utilizado */

		posicao = 0; /* Zera a posição de escrita na página, porque está
		 passando para uma nova */

		size = size - BYTES_TO_WRITE; /* Diminui a quantidade de bytes escritos do
		 montante total restante */

		HAL_Delay(5); /* Delay necessário para finalização do processo de acesso a
		 memória EEPROM */
	}
	return STATUS; /* Retorna o status final de toda a operação de escrita */
}

/*--------------------------- LER DADOS NA EEPROM -------------------------*/

/**
 * @brief Função utilizada para ler dados na memória EEPROM
 * @param pagina: Página a ser lida
 * @param posicao: Posição a ser lido
 * @retval Valor lido na memória
 */

uint8_t EEPROM_Read(uint16_t pagina, uint16_t posicao) {
	/* Endereço de acesso na memória para leitura */
	uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;

	uint8_t EEPROM_MEMORY_VALUE = 0; /* variável para armezamento do
	 valor lido */

	/* Função para envio de requisição de acesso a memória para leitura */
	HAL_I2C_Mem_Read(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS, 2,
			&EEPROM_MEMORY_VALUE, 1, 1000);

	return EEPROM_MEMORY_VALUE; /* Retorna o valor lido na memória */
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

	uint16_t ULTIMA_PAGINA = (pagina + posicao) / 128 + 1; /* Última pagina
	 acessada na leitura ou escrita */

	/* Laço para escrita sequêncial na EEPROM */
	for (int i = 0; i < ULTIMA_PAGINA; i++) {

		/* Endereço de acesso na memória para escrita */
		uint16_t EEPROM_MEMORY_ADRESS = (pagina << DESLOCAMENTO) | posicao;

		/* Quantidade de bytes a se escrever na página || máximo 128 */
		uint16_t BYTES_TO_WRITE = bytesToWriteRead(size, posicao);

		/* Função de requisição de multipla leitura I2C, junto a uma
		 * verificação de ERRO */
		if (HAL_I2C_Mem_Read(EEPROM_HANDLE_I2C, ENDERECO, EEPROM_MEMORY_ADRESS,
				2, pDados, BYTES_TO_WRITE, 1000) != HAL_OK)
			STATUS = HAL_ERROR; /* Caso algo dê errado, STATUS recebe  HAL_ERROR */

		pagina += 1; /* Incrementa 1 página, pois a função "bytesToWriteRead"
		 garante que o espaço restante total seja utilizado */

		posicao = 0; /* Zera a posição de escrita na página, porque está
		 passando para uma nova */

		size = size - BYTES_TO_WRITE; /* Diminui a quantidade de bytes escritos do
		 montante total restante */

		HAL_Delay(5); /* Delay necessário para finalização do processo de acesso a
		 memória EEPROM */
	}
	return STATUS; /* Retorna o status final de toda a operação de escrita */
}

/*---------------------- CHECAR ENDEREÇO DE COMUNICAÇÃO -------------------*/

/**
 * @brief Função utilizada para determinar o endereço de comunicação com a EEPROM
 * @param ***NONE***
 * @retval Endereço de comunicação
 */

uint16_t checkAdress() {
	/* Laço para teste de comunicação de cada endereço individualmente */
	for (int i = 0; i < 256; i++) {
		/* Para cada endereço, verifica-se se está hábil à comunicação */
		if (HAL_I2C_IsDeviceReady(&hi2c1, i, 5, 100) == HAL_OK) {
			ENDERECO = i;
			return ENDERECO; /* Caso esteja hábil à comunicação, retorna o
			endereço */
		}
	}
	return 0x00; /* Caso não tenha nenhum endereço disponível a comunicação,
	retorna 0 (0x00 == 0) */
}
