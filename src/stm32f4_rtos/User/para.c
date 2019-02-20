#include "includes.h"

DEVICE_INFO device_info;

void read_device_info(void)
{
  u8 i;
  W25QXX_Read((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  W25QXX_Read((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
//  W25QXX_Read((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
//  W25QXX_Read((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  if(device_info.isfirst != 0xab)
  {
    device_info.isfirst = 0xaa;
    sprintf((char *)device_info.device_id,"%s","jx1234");
    device_info.device_id_len = 6;
    device_info.system_state = 0;
    device_info.page_count_all = 0;
    device_info.page_count_select = 0;
    //����Ĭ�ϲ�������
    device_info.class_para.class_enable_onoff = 1;//Ĭ�Ͽ���A/B����
    device_info.class_para.class_time_hour = 8;//Ĭ�ϻ���ʱ��08:00:00
    device_info.class_para.class_time_minute = 0;
    //��Ƭ¼��Ĭ�ϲ�������
    device_info.card_count.card_A_count = 0;
    device_info.card_count.card_B_count = 0;
    device_info.card_count.card_repair_count = 0;
    //ҳ�湦��Ĭ�ϲ�������
    device_info.func_onoff.jingsha = 1;
    device_info.func_onoff.channeng = 1;
    device_info.func_onoff.weimi = 1;
    
    for(i=0;i<30;i++)
    {
      device_info.onoff[i] = 0;
    }
    for(i=0;i<10;i++)
    {
      device_info.file_select[i] = 0;
    }
    for(i=0;i<11;i++)
    {
      device_info.stop_para.stop_time[i] = 0;
    }
    device_info.weimi_info.reg = 0;
    device_info.weimi_info.count = 0;
    //������Ĭ�ϲ�������
    device_info.period_para.period_enable_onoff = 0;//Ĭ�������ڹر�
    device_info.period_para.period_year = 20;//Ĭ����������2020��
    device_info.period_para.period_month = 1;//Ĭ����������1��
    device_info.period_para.period_day = 1;//Ĭ����������1��
    memset(device_info.period_para.period_password,0,10);
    strcpy((char *)device_info.period_para.period_password,"232323");
    device_info.period_para.period_password_len = strlen((char const *)device_info.period_para.period_password);
    //ϵͳ��¼����Ĭ������
    memset(device_info.regin_in.password,0,6);
    strcpy((char *)device_info.regin_in.password,"111111");
    device_info.regin_in.password_len = strlen((char const *)device_info.regin_in.password);;  
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
    init_peiliao_para(&peiliao_para);
    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
    init_product_para(&product_para,&peiliao_para);
    W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
    init_weimi_para(&weimi_para);
    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  }
  if(device_info.page_count_all == 0)
  {
    JingSha_File.filename_len = 0;
    for(i=0;i<10;i++)
    {
      JingSha_File.filename[i] = 0;
    }
    for(i=0;i<30;i++)
    {
      JingSha_File.weight_value[i] = 0;
    }
    JingSha_File.year = 18;
    JingSha_File.month = 1;
    JingSha_File.day = 1;
    JingSha_File.week = 1;
    JingSha_File.hour = 1;
    JingSha_File.minute = 1;
    JingSha_File.second = 1;
    
    init_peiliao_para(&peiliao_para);
    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
    init_weimi_para(&weimi_para);
    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  }
  else
  {
    W25QXX_Read((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));//��ɴ��������
    W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select),sizeof(peiliao_para));//���ϱ�������
    W25QXX_Read((u8 *)&weimi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select),sizeof(weimi_para));//γ�ܱ�������
  }
}

void para_init(SLAVE_PARA *para)
{
  u8 i;
  for(i=0;i<30;i++)
  {
    para->onoff[i] = device_info.onoff[i];
    para->value_set[i] = JingSha_File.weight_value[i];
    para->value_sample[i] = 0;
  }
  para->filename_len = JingSha_File.filename_len;
  for(i=0;i<JingSha_File.filename_len;i++)
  {
    para->filename[i] = JingSha_File.filename[i];
  }
  for(i=0;i<10;i++)
    file_select[i] = device_info.file_select[i];
}

void read_from_disk(char *diskbuf)
{
  char *trx = 0;
  JINGSHA_FILE jingsha_file;
  WEIMI_PARA weimi_file;
  PEILIAO_PARA peiliao_file;
  
  jingsha_file.filename_len = strlen(Disk_File.filename[readFilenum]) - 4;                //�ļ�������  
  memcpy(jingsha_file.filename,Disk_File.filename[readFilenum],jingsha_file.filename_len);//�ļ���
  if(check_cmd(diskbuf,"λ��,�趨����"))
  {
    printf("��ʽ��ȷ\n");
    char *strx = 0;
    strx = strstr(diskbuf,"1#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[0]);
    }
    strx = strstr(diskbuf,"2#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[1]);
    }
    strx = strstr(diskbuf,"3#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[2]);
    }
    strx = strstr(diskbuf,"4#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[3]);
    }
    strx = strstr(diskbuf,"5#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[4]);
    }
    strx = strstr(diskbuf,"6#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[5]);
    }
    strx = strstr(diskbuf,"7#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[6]);
    }
    strx = strstr(diskbuf,"8#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[7]);
    }
    strx = strstr(diskbuf,"9#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[8]);
    }
    strx = strstr(diskbuf,"10#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[9]);
    }
    strx = strstr(diskbuf,"11#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[10]);
    }
    strx = strstr(diskbuf,"12#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[11]);
    }
    strx = strstr(diskbuf,"13#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[12]);
    }
    strx = strstr(diskbuf,"14#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[13]);
    }
    strx = strstr(diskbuf,"15#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[14]);
    }
    strx = strstr(diskbuf,"16#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[15]);
    }
    strx = strstr(diskbuf,"17#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[16]);
    }
    strx = strstr(diskbuf,"18#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[17]);
    }
    strx = strstr(diskbuf,"19#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[18]);
    }
    strx = strstr(diskbuf,"20#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[19]);
    }
    strx = strstr(diskbuf,"21#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[20]);
    }
    strx = strstr(diskbuf,"22#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[21]);
    }
    strx = strstr(diskbuf,"23#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[22]);
    }
    strx = strstr(diskbuf,"24#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[23]);
    }
    strx = strstr(diskbuf,"25#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[24]);
    }
    strx = strstr(diskbuf,"26#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[25]);
    }
    strx = strstr(diskbuf,"27#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[26]);
    }
    strx = strstr(diskbuf,"28#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[27]);
    }
    strx = strstr(diskbuf,"29#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[28]);
    }
    strx = strstr(diskbuf,"30#");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&jingsha_file.weight_value[29]);
    }
    strx = strstr(diskbuf,"ʱ������");
    if(strx)
    {
      sscanf(strx,"ʱ������,%d/%d/%d,%d:%d:%d\n\n",&jingsha_file.year,&jingsha_file.month,
             &jingsha_file.day,&jingsha_file.hour,&jingsha_file.minute,&jingsha_file.second);
    }
    strx = strstr(diskbuf,"��ɴ");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_file.latitude_weight);
    }
    strx = strstr(diskbuf,"γɴ");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_file.longitude_weight);
    }
    strx = strstr(diskbuf,"��");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_file.rubber_weight);
    }
    strx = strstr(diskbuf,"��Ʒ");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_file.final_weight);
    }
    strx = strstr(diskbuf,"֯������");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_file.loom_num);
    }
    strx = strstr(diskbuf,"���");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_file.loss);
    }
    strx = strstr(diskbuf,"������");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%ld",&peiliao_file.total_meter_set);
    }
    strx = strstr(diskbuf,"��������");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%ld",&peiliao_file.total_weitht_set);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_file.kaidu_set);
    }
    strx = strstr(diskbuf,"γ��");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_file.weimi_set);
    }
    strx = strstr(diskbuf,"γ����ʾ");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_file.weimi_dis_set);
    }
    strx = strstr(diskbuf,"��������");
    if(strx)
    {
      char da[10];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_file.add_meter_set);
    }
    strx = strstr(diskbuf,"һ��");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[0],&weimi_file.wei_cm_set[0],&weimi_file.wei_inch_set[0],
             &weimi_file.total_wei_count[1],&weimi_file.step1_factor[0],&weimi_file.step2_factor[0]);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[2],&weimi_file.wei_cm_set[1],&weimi_file.wei_inch_set[1],
             &weimi_file.total_wei_count[3],&weimi_file.step1_factor[1],&weimi_file.step2_factor[1]);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[4],&weimi_file.wei_cm_set[2],&weimi_file.wei_inch_set[2],
             &weimi_file.total_wei_count[5],&weimi_file.step1_factor[2],&weimi_file.step2_factor[2]);
    }
    strx = strstr(diskbuf,"�Ķ�");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[6],&weimi_file.wei_cm_set[3],&weimi_file.wei_inch_set[3],
             &weimi_file.total_wei_count[7],&weimi_file.step1_factor[3],&weimi_file.step2_factor[3]);
    }
    strx = strstr(diskbuf,"���");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[8],&weimi_file.wei_cm_set[4],&weimi_file.wei_inch_set[4],
             &weimi_file.total_wei_count[9],&weimi_file.step1_factor[4],&weimi_file.step2_factor[4]);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[10],&weimi_file.wei_cm_set[5],&weimi_file.wei_inch_set[5],
             &weimi_file.total_wei_count[11],&weimi_file.step1_factor[5],&weimi_file.step2_factor[5]);
    }
    strx = strstr(diskbuf,"�߶�");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[12],&weimi_file.wei_cm_set[6],&weimi_file.wei_inch_set[6],
             &weimi_file.total_wei_count[13],&weimi_file.step1_factor[6],&weimi_file.step2_factor[6]);
    }
    strx = strstr(diskbuf,"�˶�");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[14],&weimi_file.wei_cm_set[7],&weimi_file.wei_inch_set[7],
             &weimi_file.total_wei_count[15],&weimi_file.step1_factor[7],&weimi_file.step2_factor[7]);
    }
    strx = strstr(diskbuf,"�Ŷ�");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[16],&weimi_file.wei_cm_set[8],&weimi_file.wei_inch_set[8],
             &weimi_file.total_wei_count[17],&weimi_file.step1_factor[8],&weimi_file.step2_factor[8]);
    }
    strx = strstr(diskbuf,"ʮ��");
    if(strx)
    {
      char da[30];
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_file.total_wei_count[18],&weimi_file.wei_cm_set[9],&weimi_file.wei_inch_set[9],
             &weimi_file.total_wei_count[19],&weimi_file.step1_factor[9],&weimi_file.step2_factor[9]);
    }
  }
  
  if(device_info.page_count_all >= 10)
  {//ȥ����һ���ļ��������ļ���������
    u8 *file_read;
    file_read = mymalloc(SRAMIN,1024);
    W25QXX_Read(file_read,(u32)W25QXX_ADDR_JINGSHA + JINGSHA_SIZE,JINGSHA_SIZE * 9);//����2-10�ļ�������
    W25QXX_Write(file_read,(u32)W25QXX_ADDR_JINGSHA,JINGSHA_SIZE * 9);//��д��1-9�ļ���ַ
    W25QXX_Write((u8 *)&jingsha_file,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * 9),sizeof(jingsha_file));//д��ǰҳ�����ݵ��ļ�10��ַ
    
    //�������ݱ���
    peiliao_file.complete_meter = 0;
    peiliao_file.complete_work_time = 0;
    W25QXX_Write((u8 *)&peiliao_file,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * 9,sizeof(peiliao_file));
    
    //γ�����ݱ���
    W25QXX_Write((u8 *)&weimi_file,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * 9,sizeof(weimi_file));
    myfree(SRAMIN,file_read);
  }
  else
  {
    W25QXX_Write((u8 *)&jingsha_file,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_all),sizeof(jingsha_file));
    //�������ݱ���
    peiliao_file.complete_meter = 0;
    peiliao_file.complete_work_time = 0;
    W25QXX_Write((u8 *)&peiliao_file,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * device_info.page_count_all,sizeof(peiliao_file));
    
    //γ�����ݱ���
    W25QXX_Write((u8 *)&weimi_file,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_all,sizeof(weimi_file));
    
    device_info.page_count_all++;
    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  }
}

u8* check_cmd(char *str1,char *str2)
{
  char *strx = 0;
  strx = strstr((const char*)str1,(const char*)str2);
  return (u8*)strx;
}