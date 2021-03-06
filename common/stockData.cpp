/* 
======================================================
 Copyright 2016 Liang Ma

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
======================================================
*
* Author:   Liang Ma (liang-ma@polito.it)
*
*----------------------------------------------------------------------------
*/

#include "stockData.h"

stockData::stockData(data_t timeT, data_t freeRate, data_t volatility,
		data_t initPrice,data_t strikePrice)
{
	this->freeRate=freeRate;
	this->initPrice=initPrice;
	this->timeT=timeT;
	this->volatility=volatility;
	this->strikePrice=strikePrice;
}

stockData::stockData(const stockData& data)
{
	this->freeRate=data.freeRate;
	this->initPrice=data.initPrice;
	this->timeT=data.timeT;
	this->volatility=data.volatility;
	this->strikePrice=data.strikePrice;
}

void stockData::print()const
{
	std::cout<<"timeT:"<<timeT<<' '
			<<"freeRate:"<<freeRate<<' '
			<<"volatility:"<<volatility<<' '
			<<"initPrice:"<<initPrice<<' '
			<<"strikePrice:"<<strikePrice
			<<std::endl;
}
