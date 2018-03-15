/*
 * ThrElem.cpp
 *
 *  Created on: Apr 25, 2016
 *      Author: OS1
 */

#include "PCBElem.h"
#include "operator.h"

PCBElem::PCBElem() {
	sl=NULL;
}

PCBElem::~PCBElem() 
{
	e=NULL;
	sl=NULL;
}
