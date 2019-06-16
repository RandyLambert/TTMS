/*
* Copyright(C), 2007-2008, XUPT Univ.	 
* ������ţ�TTMS_UC_02
* File name: Seat_UI.c			  
* Description : ������λ���������	
* Author:   XUPT  		 
* Version:  v.1 	 
* Date: 	2015��4��22��	
*/

#include "Seat_UI.h"
#include "../Service/Seat.h"

#include "../Service/Studio.h"
#include "../Common/List.h"
#include <stdio.h>
#include <string.h>
/*
��ʶ����TTMS_SCU_Seat_UI_S2C 
�������ܣ�������λ״̬��ȡ������ʾ���š�
����˵����statusΪseat_status_t���ͣ���ʾ��λ״̬��
�� �� ֵ���ַ��ͣ���ʾ��λ�Ľ�����ʾ���š�
*/
inline char Seat_UI_Status2Char(seat_status_t status) {
    if(status == 0)
        return ' ';
    else if(status == 1)
	    return '#';
    else if(status == 9)
        return '@';
}

/*
��ʶ����TTMS_SCU_Seat_UI_C2S
�������ܣ�����������Ż�ȡ��λ״̬��
����˵����statusCharΪ�ַ��ͣ���ʾ������λ��������š�
�� �� ֵ��seat_status_t���ͣ���ʾ��λ��״̬��
*/
inline seat_status_t Seat_UI_Char2Status(char statusChar) {
    if(statusChar == ' ')
	    return SEAT_NONE;
    else if(statusChar == '#')
        return SEAT_GOOD;
    else if(statusChar == '@')
        return SEAT_BROKEN;
}

/*
��ʶ����TTMS_SCU_Seat_UI_MgtEnt
�������ܣ�����������λ����ں�������ʾ��ǰ����λ���ݣ����ṩ��λ�������ӡ��޸ġ�ɾ�����ܲ�������ڡ�
����˵����roomIDΪ���ͣ�����Ҫ������λ���ݳ���ID��
�� �� ֵ���ޡ�
*/ 
void Seat_UI_MgtEntry(int roomID) {

    studio_t rec;
    char choice;

    if (!Studio_Srv_FetchByID(roomID, &rec)) {
		printf("The room does not exist!\nPress [Enter] key to return!\n");
		getchar();
		return;
	}


    //printf("room = %d\n",rec.seatsCount);
    seat_list_t head;
    List_Init(head,seat_node_t); 
    if(rec.seatsCount == 0){

        rec.seatsCount = Seat_Srv_RoomInit(head,roomID,rec.rowsCount,rec.colsCount);
    }
    else{

        rec.seatsCount = Seat_Srv_FetchByRoomID(head,roomID);
    }

    Studio_Srv_Modify(&rec);
    
    printf("row:%d                   colum:%d                  seatsnumber:%d\n",rec.rowsCount,rec.colsCount,rec.seatsCount);    
    //printf("   %d   \n",head->next->data.id);
    
    printf("-------------------------------------------------------------------\n");
    setbuf(stdin,NULL);
    for(int i=1;i<=rec.rowsCount;i++)
    {
        for(int j=1;j<=rec.colsCount;j++)
        {
            int flag=0;
            seat_list_t temp;
            List_ForEach(head,temp)
            {
                if(temp->data.row==i && temp->data.column==j)                  
                {
                    flag = 1;
                    printf("%c",Seat_UI_Status2Char(temp->data.status));
                    break;
                }
            }
            if(!flag) printf(" ");//此处没有座位
        }
        putchar('\n');
    }
    //用于显示座位情况列表


    int row1,column1;
    printf(
			"******************************************************************\n");
	printf(
			"[A]dd  |  [M]od  |  [D]el  |  [R]eturn");
	printf(
			"\n==================================================================\n");
 
    printf("Your Choice:");
    setbuf(stdin,NULL);
	scanf("%c", &choice);
    setbuf(stdin,NULL);
    switch(choice)
    {
        case 'A':
        case 'a':
        printf("please input the row :");
        scanf("%d",&row1);
        setbuf(stdin,NULL);
        printf("\nplease input the col :");
        scanf("%d",&column1);
        setbuf(stdin,NULL);
        if(!Seat_UI_Add(head,roomID,row1,column1))
        {
            printf("input eor,添加失败\n");
        }
        else{
            rec.seatsCount = rec.seatsCount+1;
            Studio_Srv_Modify(&rec);
            printf("add accept\n");
        }
        break;
        case 'M':
        case 'm':
        printf("please input the row :");
        scanf("%d",&row1);
        setbuf(stdin,NULL);
        printf("\nplease input the col :");
        scanf("%d",&column1);
        setbuf(stdin,NULL);
        if(!Seat_UI_Modify(head,row1,column1))
        {
            printf("input eor，修改失败\n");
        }
        else{
             printf("mod accept\n");
        }
        break;
        case 'D':
        case 'd':
        printf("please input the row :");
        scanf("%d",&row1);
        setbuf(stdin,NULL);
        printf("\nplease input the col :");
        scanf("%d",&column1);
        setbuf(stdin,NULL);
        if(!Seat_UI_Delete(head,row1,column1))
        {
            printf("input eor，删除失败\n");
        }
        else{
            rec.seatsCount = rec.seatsCount-1;
            Studio_Srv_Modify(&rec);
            printf("del accept\n");
        }
        break;
        default: break;
    }
}

/*
ʶ����TTMS_SCU_Seat_UI_Add
�������ܣ���������һ���µ���λ���ݡ�
����˵������һ������listΪseat_list_t����ָ�룬ָ����λ����ͷָ�룬
         �ڶ�������rowsCountΪ���ͣ���ʾ��λ�����кţ�����������colsCountΪ���ͣ���ʾ��λ�����кš�
�� �� ֵ�����ͣ���ʾ�Ƿ�ɹ���������λ�ı�־��
*/

int Seat_UI_Add(seat_list_t list, int roomID, int row, int column) {  //����һ����λ

    seat_list_t tmp = Seat_Srv_FindByRowCol(list,row,column);
    if(tmp == NULL){
        seat_t temp;

        temp.column = column;
        temp.row = row;
        temp.status = SEAT_GOOD;
        temp.roomID = roomID;
        
        if(Seat_Srv_Add(&temp)){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

/*
��ʶ����TTMS_SCU_Seat_UI_Mod 
�������ܣ������޸�һ����λ���ݡ�
����˵������һ������listΪseat_list_t����ָ�룬ָ����λ����ͷָ�룬�ڶ�������rowsCountΪ���ͣ���ʾ��λ�����кţ�����������colsCountΪ���ͣ���ʾ��λ�����кš�
�� �� ֵ�����ͣ���ʾ�Ƿ�ɹ��޸�����λ�ı�־��
*/
int Seat_UI_Modify(seat_list_t list, int row, int column) {
    
    seat_list_t tmp = Seat_Srv_FindByRowCol(list,row,column);
    
    if(tmp!=NULL){
        seat_t temp;
        Seat_Perst_SelectByID(tmp->data.id,&temp);
        char choice;
        printf(
	    		"*******************************************************************\n");
	    printf(
	    		"[#]Good | [@]Break | [ ]Empty | the seat's situation is %c",Seat_UI_Status2Char(temp.status));
	    printf(
	    		"\n==================================================================\n");
        printf("Your Choice:");
        setbuf(stdin,NULL);
	    scanf("%c", &choice);
        setbuf(stdin,NULL);
        tmp->data.status = Seat_UI_Char2Status(choice);

        if(Seat_Srv_Modify(&tmp->data)){

            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}

/*
��ʶ����TTMS_SCU_Seat_UI_Del
�������ܣ�����ɾ��һ����λ�����ݡ�
����˵������һ������listΪseat_list_t����ָ�룬ָ����λ����ͷָ�룬�ڶ�������rowsCountΪ���ͣ���ʾ��λ�����кţ�����������colsCountΪ���ͣ���ʾ��λ�����кš�
�� �� ֵ�����ͣ���ʾ�Ƿ�ɹ�ɾ������λ�ı�־��
*/
int Seat_UI_Delete(seat_list_t list, int row, int column) {
    
    seat_list_t tmp = Seat_Srv_FindByRowCol(list,row,column);
    
    if(tmp!=NULL){
        if(Seat_Srv_DeleteByID(tmp->data.id)){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }  
}