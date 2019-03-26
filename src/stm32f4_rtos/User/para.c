#include "includes.h"

DEVICE_INFO device_info;

void read_device_info(void)
{
  u8 i;
  W25QXX_Read((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
//  W25QXX_Read((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
  if(device_info.isfirst != 0xaa)
  {
    device_info.isfirst = 0xaa;
    sprintf((char *)device_info.device_id,"%s","jx1234");
    device_info.device_id_len = 6;
    device_info.system_state = SYS_NORMAL;
    device_info.page_count_all = 0;
    device_info.page_count_select = 0;
    device_info.sevro_stop_pluse = 1000;
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
    
    device_info.ratio.GEAR1 = 60;
    device_info.ratio.GEAR2 = 30;  
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
    device_info.weimi_info.songwei_seg[0] = 0;
    device_info.weimi_info.songwei_seg[1] = 0;
    device_info.weimi_info.songwei_seg[2] = 0;
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
    
    init_jingsha_para(&JingSha_File);
    W25QXX_Write((u8 *)&JingSha_File,(u32)W25QXX_ADDR_JINGSHA,sizeof(JingSha_File));
    init_peiliao_para(&peiliao_para);
    W25QXX_Write((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
    init_product_para(&product_para,peiliao_para);
    W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
    init_weimi_para(&weimi_para,peiliao_para);
    W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  }
  else
  {
    W25QXX_Read((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));//��ɴ��������
    W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select),sizeof(peiliao_para));//���ϱ�������
    W25QXX_Read((u8 *)&weimi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select),sizeof(weimi_para));//γ�ܱ�������
    W25QXX_Read((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
  }
}

void default_device_para(void)
{
  u8 i;
  
  device_info.isfirst = 0xaa;
  sprintf((char *)device_info.device_id,"%s","jx1234");
  device_info.device_id_len = 6;
  device_info.system_state = SYS_NORMAL;
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
  
  device_info.ratio.GEAR1 = 35;
  device_info.ratio.GEAR2 = 10;  
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
  init_product_para(&product_para,peiliao_para);
  W25QXX_Write((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
  init_weimi_para(&weimi_para,peiliao_para);
  W25QXX_Write((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  
  JingSha_File.filename_len = 0;
  for(i=0;i<10;i++)
  {
    JingSha_File.filename[i] = 0;
  }
  for(i=0;i<30;i++)
  {
    JingSha_File.weight_value[i] = 0;
  }
  para_init(&SlavePara);
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

//��ȡU�����ݵ�ϵͳ
void read_from_disk(char *diskbuf)
{
  if(check_cmd(diskbuf,"λ��,�趨����"))
  {
    printf("��ʽ��ȷ\n");
    char *strx = 0;
    char da[50];
    strx = strstr(diskbuf,"1#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[0] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"2#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[1] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"3#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[2] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"4#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[3] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"5#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[4] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"6#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[5] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"7#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[6] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"8#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[7] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"9#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[8] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"10#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[9] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"11#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[10] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"12#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[11] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"13#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[12] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"14#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[13] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"15#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[14] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"16#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[15] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"17#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[16] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"18#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[17] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"19#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[18] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"20#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[19] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"21#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[20] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"22#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[21] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"23#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[22] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"24#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[23] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"25#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[24] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"26#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[25] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"27#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[26] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"28#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[27] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"29#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[28] = (u16)(value * 1000);
    }
    strx = strstr(diskbuf,"30#");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      float value;
      sscanf(da,"%f",&value);
      SlavePara.value_set[29] = (u16)(value * 1000);
    }
//    strx = strstr(diskbuf,"ʱ������");
//    if(strx)
//    {
//      sscanf(strx,"ʱ������,%d/%d/%d,%d:%d:%d\n\n",&jingsha_file.year,&jingsha_file.month,
//             &jingsha_file.day,&jingsha_file.hour,&jingsha_file.minute,&jingsha_file.second);
//    }
    strx = strstr(diskbuf,"��ɴ");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.latitude_weight);
    }
    strx = strstr(diskbuf,"γɴ");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.longitude_weight);
    }
    strx = strstr(diskbuf,"��");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.rubber_weight);
    }
    strx = strstr(diskbuf,"��Ʒ");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.final_weight);
    }
    strx = strstr(diskbuf,"֯������");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.loom_num);
    }
    strx = strstr(diskbuf,"���");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.loss);
    }
    strx = strstr(diskbuf,"������");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%ld",&peiliao_para.total_meter_set);
    }
    strx = strstr(diskbuf,"��������");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%ld",&peiliao_para.total_weitht_set);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.kaidu_set);
    }
    strx = strstr(diskbuf,"γ��");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.weimi_set);
    }
    strx = strstr(diskbuf,"γ����ʾ");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.weimi_dis_set);
    }
    strx = strstr(diskbuf,"��������");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.add_meter_set);
    }
    strx = strstr(diskbuf,"һ��");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[0],&weimi_para.wei_cm_set[0],&weimi_para.wei_inch_set[0],
             &weimi_para.total_wei_count[1],&weimi_para.step_factor[0][0],&weimi_para.step_factor[1][0],&weimi_para.step_factor[2][0]);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[2],&weimi_para.wei_cm_set[1],&weimi_para.wei_inch_set[1],
             &weimi_para.total_wei_count[3],&weimi_para.step_factor[0][1],&weimi_para.step_factor[1][1],&weimi_para.step_factor[2][1]);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[4],&weimi_para.wei_cm_set[2],&weimi_para.wei_inch_set[2],
             &weimi_para.total_wei_count[5],&weimi_para.step_factor[0][2],&weimi_para.step_factor[1][2],&weimi_para.step_factor[2][2]);
    }
    strx = strstr(diskbuf,"�Ķ�");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[6],&weimi_para.wei_cm_set[3],&weimi_para.wei_inch_set[3],
             &weimi_para.total_wei_count[7],&weimi_para.step_factor[0][3],&weimi_para.step_factor[1][3],&weimi_para.step_factor[2][3]);
    }
    strx = strstr(diskbuf,"���");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[8],&weimi_para.wei_cm_set[4],&weimi_para.wei_inch_set[4],
             &weimi_para.total_wei_count[9],&weimi_para.step_factor[0][4],&weimi_para.step_factor[1][4],&weimi_para.step_factor[2][4]);
    }
    strx = strstr(diskbuf,"����");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[10],&weimi_para.wei_cm_set[5],&weimi_para.wei_inch_set[5],
             &weimi_para.total_wei_count[11],&weimi_para.step_factor[0][5],&weimi_para.step_factor[1][5],&weimi_para.step_factor[2][5]);
    }
    strx = strstr(diskbuf,"�߶�");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[12],&weimi_para.wei_cm_set[6],&weimi_para.wei_inch_set[6],
             &weimi_para.total_wei_count[13],&weimi_para.step_factor[0][6],&weimi_para.step_factor[1][6],&weimi_para.step_factor[2][6]);
    }
    strx = strstr(diskbuf,"�˶�");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[14],&weimi_para.wei_cm_set[7],&weimi_para.wei_inch_set[7],
             &weimi_para.total_wei_count[15],&weimi_para.step_factor[0][7],&weimi_para.step_factor[1][7],&weimi_para.step_factor[2][7]);
    }
    strx = strstr(diskbuf,"�Ŷ�");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&weimi_para.total_wei_count[16],&weimi_para.wei_cm_set[8],&weimi_para.wei_inch_set[8],
             &weimi_para.total_wei_count[17],&weimi_para.step_factor[0][8],&weimi_para.step_factor[1][8],&weimi_para.step_factor[2][8]);
    }
    strx = strstr(diskbuf,"ʮ��");
    if(strx)
    {
      u32 total_wei[2];
      float wei_cm,wei_inch;
      u16 step[3]; 
      sscanf(strx,"%*[^,],%[^\n]",da);
      //ֱ�ӽ����ݴ��䵽γ��ʱ�����������������������ʱ��������
      sscanf(da,"%d,%f,%f,%d,%d,%d,%d",&total_wei[0],&wei_cm,&wei_inch,
             &total_wei[1],&step[0],&step[1],&step[2]);
      weimi_para.total_wei_count[18] = total_wei[0];
      weimi_para.total_wei_count[19] = total_wei[1];
      weimi_para.wei_cm_set[9] = wei_cm;
      weimi_para.wei_inch_set[9] = wei_inch;
      weimi_para.step_factor[0][9] = step[0];
      weimi_para.step_factor[1][9] = step[1];
      weimi_para.step_factor[2][9] = step[2];
    }
  }
}

u8* check_cmd(char *str1,char *str2)
{
  char *strx = 0;
  strx = strstr((const char*)str1,(const char*)str2);
  return (u8*)strx;
}