/*
 *      Armazenamento Externo EEPROM - EEPROM.h
 *
 *      Data: 12 de agosto, 2023
 *      Autor: Gabriel Luiz
 *      Contato: (31) 97136-4334 || gabrielluiz.eletro@gmail.com
 */
/*
 * 		- Links Úteis -
 *
 */

#ifndef SRC_EEPROM_H_
#define SRC_EEPROM_H_

#include "stm32f1xx_hal.h"

/*----------------------- BYTES PARA LER OU ESCREVER ----------------------*/

/**
 * @brief Função usada para determinar a quantidade de bytes a escrever ou ler
 * @param Tamanho: Quantidade de bytes
 * @param Posicao: Posição de escrita na página
 * @param tamanho_pagina: Tamanho de uma unica página
 * @retval Quantidade de bytes para se escrever na página
 */

void EEPROM_Init(uint16_t Paginas_d, uint16_t Tamanho_pagina_d);

/*------------------------- INICIALIZAR A EEPROM --------------------------*/

/**
 * @brief Função utilizada para inicializar corretamente a biblioteca
 * @param Paginas_d: Quantidade total de páginas
 * @param Tamanho_pagina_d:  Tamanho total de cada página
 * @retval ***NONE***
 */

uint8_t EEPROM_Write(uint16_t pagina, uint16_t posicao, uint8_t valor);

/*------------------------ ESCREVER DADOS NA EEPROM ----------------------*/

/**
 * @brief Função utilizada para escrita na memória EEPROM
 * @param pagina: Página a ser escrita
 * @param posicao: Posição a ser escrito
 * @param valor: Valor a ser escrito na memória
 * @retval Confirmação do valor escrito
 */

HAL_StatusTypeDef EEPROM_Write_MultiData(uint16_t pagina, uint16_t posicao,
		uint8_t *pDados, uint16_t size);

/*--------------------------- LER DADOS NA EEPROM -------------------------*/

/**
 * @brief Função utilizada para ler dados na memória EEPROM
 * @param pagina: Página a ser lida
 * @param posicao: Posição a ser lido
 * @retval Valor lido na memória
 */

uint8_t EEPROM_Read(uint16_t pagina, uint16_t posicao);

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
		uint8_t *pDados, uint16_t size);

/*---------------------- CHECAR ENDEREÇO DE COMUNICAÇÃO -------------------*/

/**
 * @brief Função utilizada para determinar o endereço de comunicação com a EEPROM
 * @param ***NONE***
 * @retval Endereço de comunicação
 */

uint16_t checkAdress();

/*-------------------------------------------------------------------------*/

#endif /* SRC_EEPROM_H_ */
