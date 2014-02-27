
#ifndef _IOOperation_H_
#define _IOOperation_H_

namespace Framework
{

	typedef enum IO_Operation{
		IO_Accept_Completed,
		IO_Disconnect_Completed,
		IO_Read_Completed,
		IO_Write_Completed,
		IO_Termination
	}IO_Operation;

}

#endif