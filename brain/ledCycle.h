#ifndef _LED_CYCLE_
#define _LED_CYCLE_

#include "cmsis_os2.h"
#include "gpio.h"
#include "common.h"

/**
  * Use cases: (all these functions return ids and allocate memory to the threads)
  * runLedCycle1Thread();    --> just a cycle
	*  runLedFlash500Thread(); --> flash at rate 500ms
	*  runLedFlash200Thread(); --> flash at rate 250 (typing error but it's 250)
	*   turnOnLed();           --> turn on all leds, added small delay to return control back and reduce cycles
	*/

/**
 * MEMORY ALLOCATION
 */
 //////////////////////////////////
 
//stick of 64 * 8 bytes = 512 bytes of Cortex stack memory for array of Nodes
//to allocate memory to thread, use osThreadNew(name, NULL, &ledcycle_attr
static uint64_t ledcycle_stk1[64];
static uint64_t ledflash_stk[64];
static uint64_t ledflash_stk1[64];
static uint64_t ledon_stl[64];

const osThreadAttr_t ledcycle_attr = {
  .stack_mem  = &ledcycle_stk1[0],
  .stack_size = sizeof(ledcycle_stk1)
	};
const osThreadAttr_t ledflash_attr = {
	.stack_mem = &ledflash_stk[0],
	.stack_size = sizeof(ledflash_stk)
	};
const osThreadAttr_t ledflash_attr1 = {
	.stack_mem = &ledflash_stk1[0],
	.stack_size = sizeof(ledflash_stk1)
	};
const osThreadAttr_t ledon_attr = {
	.stack_mem = &ledon_stl[0],
	.stack_size = sizeof(ledon_stl)
};

typedef struct node {
	int pinNo;
	PORT_Type* PORT;	//PORT
}ledArrayNode;

/**
 * Initialize led node
 */
ledArrayNode initNode(int pinNo, PORT_Type* PORT) {
	ledArrayNode res;
	res.pinNo = pinNo;
	res.PORT = PORT;
	
	GPIOInitOutput(PORT, pinNo);
	return res;
}

/**
 * HELPER FUNCTIONS
 */
 //////////////////////////////////////////////
/**
 * ON -> DELAY -> OFF
 */
void pulse(ledArrayNode node) {
	GPIOSetOutput(node.PORT, node.pinNo, HIGH);
	osDelay(50);
	GPIOSetOutput(node.PORT, node.pinNo, LOW);
}


/**
 *	For bottom left pins to cycle thread
 */
void ledRun1(void* argument) {
	ledArrayNode node0 = initNode(13, PORTA);
	ledArrayNode node1 = initNode(5, PORTD);
	ledArrayNode node2 = initNode(0, PORTD);
	ledArrayNode node3 = initNode(2, PORTD);
	ledArrayNode node4 = initNode(3, PORTD);
	ledArrayNode node5 = initNode(16, PORTA);
	ledArrayNode node6 = initNode(17, PORTC);
	ledArrayNode node7 = initNode(16, PORTC);
	ledArrayNode node8 = initNode(13, PORTC);
	ledArrayNode node9 = initNode(12, PORTC);
	
	//construct list of nodes
	ledArrayNode nodeArray[10] = {node0, node1, node2, node3, node4, node5, node6, node7, node8, node9};
		for (;;) {
			osEventFlagsWait(flagRunning, 0x01, osFlagsNoClear, osWaitForever);
			for (int x = 0; x < 10; ++x) {
				pulse(nodeArray[x]);
			}
		}
}

/**
 * FLASHING
 */
 //////////////////////////////////////////////////////
void flashLed(ledArrayNode* arr, uint32_t delay) {
	for (int x = 0; x < 10; ++x) {
		GPIOSetOutput(arr[x].PORT, arr[x].pinNo,HIGH);
	}
	osDelay(delay);
	for (int y = 0; y < 10; ++y) {
		GPIOSetOutput(arr[y].PORT, arr[y].pinNo, LOW);
	}
	osDelay(delay);
}

void ledFlash500(void *argument) {
	ledArrayNode node0 = initNode(21, PORTE);
	ledArrayNode node1 = initNode(20, PORTE);
	ledArrayNode node2 = initNode(5, PORTE);
	ledArrayNode node3 = initNode(4, PORTE);
	ledArrayNode node4 = initNode(3, PORTE);
	ledArrayNode node5 = initNode(2, PORTE);
	ledArrayNode node6 = initNode(11, PORTB);
	ledArrayNode node7 = initNode(10, PORTB);
	ledArrayNode node8 = initNode(9, PORTB);
	ledArrayNode node9 = initNode(8, PORTB);
	//construct list of nodes
	ledArrayNode nodeArray[10] = {node0, node1, node2, node3, node4, node5, node6, node7, node8, node9};
	for (;;) {
		osEventFlagsWait(flagRunning, 0x01, osFlagsNoClear, osWaitForever);
		flashLed(nodeArray, 500);
	}
}

void ledFlash250(void *argument) {
	ledArrayNode node0 = initNode(21, PORTE);
	ledArrayNode node1 = initNode(20, PORTE);
	ledArrayNode node2 = initNode(5, PORTE);
	ledArrayNode node3 = initNode(4, PORTE);
	ledArrayNode node4 = initNode(3, PORTE);
	ledArrayNode node5 = initNode(2, PORTE);
	ledArrayNode node6 = initNode(11, PORTB);
	ledArrayNode node7 = initNode(10, PORTB);
	ledArrayNode node8 = initNode(9, PORTB);
	ledArrayNode node9 = initNode(8, PORTB);
	//construct list of nodes
	ledArrayNode nodeArray[10] = {node0, node1, node2, node3, node4, node5, node6, node7, node8, node9};
	for (;;) {
		osEventFlagsWait(flagStation, 0x01, osFlagsNoClear, osWaitForever);
		flashLed(nodeArray, 250);
	}
}

void onLED(ledArrayNode* arr) {
	for (int x = 0; x < 10; ++x) {
		GPIOSetOutput(arr[x].PORT, arr[x].pinNo, HIGH);
	}
}
void onALLLED(void *argument) {
	ledArrayNode node0 = initNode(13, PORTA);
	ledArrayNode node1 = initNode(5, PORTD);
	ledArrayNode node2 = initNode(0, PORTD);
	ledArrayNode node3 = initNode(2, PORTD);
	ledArrayNode node4 = initNode(3, PORTD);
	ledArrayNode node5 = initNode(16, PORTA);
	ledArrayNode node6 = initNode(17, PORTC);
	ledArrayNode node7 = initNode(16, PORTC);
	ledArrayNode node8 = initNode(13, PORTC);
	ledArrayNode node9 = initNode(12, PORTC);
	//construct list of nodes
	ledArrayNode nodeArray[10] = {node0, node1, node2, node3, node4, node5, node6, node7, node8, node9};


	//leave output on
	for (;;) {
		osEventFlagsWait(flagStation, 0x01, osFlagsNoClear, osWaitForever);
		onLED(nodeArray);
		osDelay(2000);
	}

}


/**
 * CREATE FUNCTIONS TO CONTROL LEDS BASED ON MOVEMENT
 */

static ledArrayNode* frontLeds;
static ledArrayNode* backLeds;

void initFrontLed(void) {
	ledArrayNode node0 = initNode(13, PORTA);
	ledArrayNode node1 = initNode(5, PORTD);
	ledArrayNode node2 = initNode(0, PORTD);
	ledArrayNode node3 = initNode(2, PORTD);
	ledArrayNode node4 = initNode(3, PORTD);
	ledArrayNode node5 = initNode(16, PORTA);
	ledArrayNode node6 = initNode(17, PORTC);
	ledArrayNode node7 = initNode(16, PORTC);
	ledArrayNode node8 = initNode(13, PORTC);
	ledArrayNode node9 = initNode(12, PORTC);
	ledArrayNode frontLedArray[10] = {node0, node1, node2,
																		node3, node4, node5,
																		node6, node7, node8, 
																		node9};
	frontLeds = frontLedArray;
}

void initBackLed(void) {
	ledArrayNode node0 = initNode(21, PORTE);
	ledArrayNode node1 = initNode(20, PORTE);
	ledArrayNode node2 = initNode(5, PORTE);
	ledArrayNode node3 = initNode(4, PORTE);
	ledArrayNode node4 = initNode(3, PORTE);
	ledArrayNode node5 = initNode(2, PORTE);
	ledArrayNode node6 = initNode(11, PORTB);
	ledArrayNode node7 = initNode(10, PORTB);
	ledArrayNode node8 = initNode(9, PORTB);
	ledArrayNode node9 = initNode(8, PORTB);
	
	//construct list of nodes
	ledArrayNode backLedArray[10] = {node0, node1, node2,
																node3, node4, node5,
																node6, node7, node8,
																node9};
	backLeds = backLedArray;
}

void turnOnLeds(void) {
	for (int x = 0; x < 10; ++x) {
		GPIOSetOutput(frontLeds[x].PORT, frontLeds[x].pinNo, HIGH);
	}
}

 
/**
 * Thread creation
 * @author DN
 */
 /////////////////////////////////////////////////////
osThreadId_t runLedCycle1Thread(void) {
	return osThreadNew(ledRun1, NULL, &ledcycle_attr);
}

osThreadId_t runLedFlash500Thread(void) {
	return osThreadNew(ledFlash500, NULL, &ledflash_attr);
}

osThreadId_t runLedFlash200Thread(void) {
	return osThreadNew(ledFlash250, NULL, &ledflash_attr1);
}

osThreadId_t turnOnLed(void) {
	return osThreadNew(onALLLED, NULL, &ledon_attr);
}
#endif 
