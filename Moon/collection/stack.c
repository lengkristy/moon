#include "stack.h"
#include <stdlib.h>
#include <malloc.h>

/**
 * 功能：初始化栈
 * 返回值：如果成功，则返回栈的地址，如果失败返回NULL
 */
Stack* Stack_Init()
{
	long i = 0;
	Stack* stack = NULL;
	stack = (Stack*) malloc(sizeof(Stack));
	if(stack == NULL)
	{
		return NULL;
	}
	//分配栈空间
	stack->node = (Stack_Node*)malloc(STACK_INIT_SIZE * sizeof(Stack_Node));
	if(stack->node == NULL)
	{
		free(stack);
		return NULL;
	}
	stack->base = 0;
	stack->top = stack->base;
	stack->size = STACK_INIT_SIZE;
	//将数据域置为空
	for(i = 0;i < stack->size;i++)
	{
		stack->node[i].data = NULL;
	}
	return stack;
}

/**
 * 功能：将数据压栈
 * 参数：
 *		stack：栈指针
 *		data：被压入栈的数据地址
 */
void Stack_Push(Stack* stack,void* data)
{
	int i = 0;
	if(stack == NULL || stack->node == NULL)
	{
		return;
	}
	//判断栈空间是否存满
	if(stack->top - stack->base + 1 == stack->size)
	{
		//重新分配占空间
		stack->node = (Stack_Node*)realloc(stack->node,(stack->size + STACK_INCREASE_SIZE) * sizeof(Stack_Node));
		if(stack->node == NULL)
		{
			return;
		}
		stack->size += STACK_INCREASE_SIZE;
		//将重新分配的数据域置为NULL
		for(i = stack->top + 1;i < stack->size;i++)
		{
			stack->node[i].data = NULL;
		}
	}
	//开始压栈
	stack->node[stack->top].data = data;
	stack->top++;
}

/**
 * 功能：将数据弹出栈
 * 参数：
 *		stack：栈指针
 * 返回值：返回的数据的地址，如果栈没有数据则返回NULL
 */
void* Stack_Pop(Stack* stack)
{
	void* data = NULL;
	if(stack == NULL || stack->node == NULL)
	{
		return NULL;
	}
	if(stack->base == stack->top)//没有数据
		return NULL;
	data = stack->node[stack->top - 1].data;//弹出栈顶内容
	stack->node[stack->top].data = NULL;//栈顶数据域置空
	stack->top--;//栈顶减1
	return data;
}

/**
 * 功能：清空栈
 * 参数：
 *		stack：栈指针
 */
void Stack_Clear(Stack* stack)
{
	long i = 0;
	if(stack == NULL || stack->node == NULL)
	{
		return;
	}
	for(i= stack->top - 1;i >= stack->base;i--)
	{
		if(i < 0)//索引不能小于0
			break;
		stack->node[i].data = NULL;
	}
	stack->top = stack->base;
}

/**
 * 功能：释放栈
 * 参数：
 *		stack：栈指针
 */
void Stack_Free(Stack* stack)
{
	if(stack == NULL)
		return;
	if(stack->node != NULL)
	{
		free(stack->node);
	}
	free(stack);
}

/**
 * 功能：栈测试
 */
void Stack_Test()
{
	int *p = NULL;
	Stack* stack = Stack_Init();
	int i = 10,j = 11,k = 12,l = 13;
	Stack_Push(stack,&i);
	Stack_Push(stack,&j);
	Stack_Push(stack,&k);
	Stack_Push(stack,&l);
	p = (int*)Stack_Pop(stack);
	//清空栈
	Stack_Clear(stack);

	//循环压入1000个数据
	for(i = 0;i < 1000;i++)
	{
		p = (int*)malloc(sizeof(int));
		*p = i;
		Stack_Push(stack,p);
	}
	p = (int*)Stack_Pop(stack);
	Stack_Clear(stack);
	//释放数据内存
	for(i = 0;i < 1000;i++)
	{
		free((int*)Stack_Pop(stack));
	}
	//清空栈
	Stack_Clear(stack);
	//10进制转8进制测试
	i = 2386;
	while(i != 0 )
	{
		p = (int*)malloc(sizeof(int));
		*p = i % 8;
		Stack_Push(stack,p);
		i = i / 8;
	}
	//依次弹出栈的数字就是八进制
	while(stack->base != stack->top)
	{
		p = (int*)Stack_Pop(stack);
		if((stack->top - stack->base + 1) == 4)
		{
			i += (*p) * 1000;
		}
		else if((stack->top - stack->base + 1) == 3)
		{
			i += (*p) * 100;
		}
		else if((stack->top - stack->base + 1) == 2)
		{
			i += (*p) * 10;
		}
		else if((stack->top - stack->base + 1) == 1)
		{
			i += (*p);
		}
		free(p);
	}
	//i的值就是2386的八进制
	Stack_Free(stack);
}