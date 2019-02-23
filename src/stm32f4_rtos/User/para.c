#include "includes.h"

DEVICE_INFO device_info;

void read_device_info(void)
{
  u8 i;
  W25QXX_Read((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
  W25QXX_Read((u8 *)&product_para,(u32)W25QXX_ADDR_CHANNENG,sizeof(product_para));
//  W25QXX_Read((u8 *)&peiliao_para,(u32)W25QXX_ADDR_PEILIAO,sizeof(peiliao_para));
//  W25QXX_Read((u8 *)&weimi_para,(u32)W25QXX_ADDR_WEIMI,sizeof(weimi_para));
  if(device_info.isfirst != 0xaa)
  {
    device_info.isfirst = 0xaa;
    sprintf((char *)device_info.device_id,"%s","jx1234");
    device_info.device_id_len = 6;
    device_info.system_state = 0;
    device_info.page_count_all = 0;
    device_info.page_count_select = 0;
    //换班默认参数设置
    device_info.class_para.class_enable_onoff = 1;//默认开启A/B换班
    device_info.class_para.class_time_hour = 8;//默认换班时间08:00:00
    device_info.class_para.class_time_minute = 0;
    //卡片录入默认参数设置
    device_info.card_count.card_A_count = 0;
    device_info.card_count.card_B_count = 0;
    device_info.card_count.card_repair_count = 0;
    //页面功能默认参数设置
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
    //试用期默认参数设置
    device_info.period_para.period_enable_onoff = 0;//默认试用期关闭
    device_info.period_para.period_year = 20;//默认试用期限2020年
    device_info.period_para.period_month = 1;//默认试用期限1月
    device_info.period_para.period_day = 1;//默认试用期限1日
    memset(device_info.period_para.period_password,0,10);
    strcpy((char *)device_info.period_para.period_password,"232323");
    device_info.period_para.period_password_len = strlen((char const *)device_info.period_para.period_password);
    //系统登录密码默认设置
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
    W25QXX_Read((u8 *)&JingSha_File,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_select),sizeof(JingSha_File));//经纱保存数据
    W25QXX_Read((u8 *)&peiliao_para,(u32)(W25QXX_ADDR_PEILIAO + PEILIAO_SIZE * device_info.page_count_select),sizeof(peiliao_para));//胚料保存数据
    W25QXX_Read((u8 *)&weimi_para,(u32)(W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_select),sizeof(weimi_para));//纬密保存数据
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

//读取U盘数据到系统
void read_from_disk(char *diskbuf)
{
//  JINGSHA_FILE jingsha_file;
//  WEIMI_PARA weimi_file;
//  PEILIAO_PARA peiliao_file;
  
//  u8 namebuf[10];
//  memset(namebuf,0,10);
//  sscanf(Disk_File.filename[readFilenum],"%*[^-]-%[^.]",namebuf);//过滤掉文件名中ID和格式，如文件名为jx1234-1122.CSV，则过滤后为1122
//  jingsha_file.filename_len = strlen(namebuf); //取文件名长度  
//  memcpy(jingsha_file.filename,namebuf,jingsha_file.filename_len);//文件名
  if(check_cmd(diskbuf,"位置,设定张力"))
  {
    printf("格式正确\n");
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
//    strx = strstr(diskbuf,"时间日期");
//    if(strx)
//    {
//      sscanf(strx,"时间日期,%d/%d/%d,%d:%d:%d\n\n",&jingsha_file.year,&jingsha_file.month,
//             &jingsha_file.day,&jingsha_file.hour,&jingsha_file.minute,&jingsha_file.second);
//    }
    strx = strstr(diskbuf,"经纱");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.latitude_weight);
    }
    strx = strstr(diskbuf,"纬纱");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.longitude_weight);
    }
    strx = strstr(diskbuf,"橡胶");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.rubber_weight);
    }
    strx = strstr(diskbuf,"成品");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.final_weight);
    }
    strx = strstr(diskbuf,"织机条数");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.loom_num);
    }
    strx = strstr(diskbuf,"损耗");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.loss);
    }
    strx = strstr(diskbuf,"米任务");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%ld",&peiliao_para.total_meter_set);
    }
    strx = strstr(diskbuf,"重量任务");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%ld",&peiliao_para.total_weitht_set);
    }
    strx = strstr(diskbuf,"开度");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.kaidu_set);
    }
    strx = strstr(diskbuf,"纬密");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%f",&peiliao_para.weimi_set);
    }
    strx = strstr(diskbuf,"纬密显示");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.weimi_dis_set);
    }
    strx = strstr(diskbuf,"补单数量");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d",&peiliao_para.add_meter_set);
    }
    strx = strstr(diskbuf,"一段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[0],&weimi_para.wei_cm_set[0],&weimi_para.wei_inch_set[0],
             &weimi_para.total_wei_count[1],&weimi_para.step1_factor[0],&weimi_para.step2_factor[0]);
    }
    strx = strstr(diskbuf,"二段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[2],&weimi_para.wei_cm_set[1],&weimi_para.wei_inch_set[1],
             &weimi_para.total_wei_count[3],&weimi_para.step1_factor[1],&weimi_para.step2_factor[1]);
    }
    strx = strstr(diskbuf,"三段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[4],&weimi_para.wei_cm_set[2],&weimi_para.wei_inch_set[2],
             &weimi_para.total_wei_count[5],&weimi_para.step1_factor[2],&weimi_para.step2_factor[2]);
    }
    strx = strstr(diskbuf,"四段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[6],&weimi_para.wei_cm_set[3],&weimi_para.wei_inch_set[3],
             &weimi_para.total_wei_count[7],&weimi_para.step1_factor[3],&weimi_para.step2_factor[3]);
    }
    strx = strstr(diskbuf,"五段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[8],&weimi_para.wei_cm_set[4],&weimi_para.wei_inch_set[4],
             &weimi_para.total_wei_count[9],&weimi_para.step1_factor[4],&weimi_para.step2_factor[4]);
    }
    strx = strstr(diskbuf,"六段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[10],&weimi_para.wei_cm_set[5],&weimi_para.wei_inch_set[5],
             &weimi_para.total_wei_count[11],&weimi_para.step1_factor[5],&weimi_para.step2_factor[5]);
    }
    strx = strstr(diskbuf,"七段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[12],&weimi_para.wei_cm_set[6],&weimi_para.wei_inch_set[6],
             &weimi_para.total_wei_count[13],&weimi_para.step1_factor[6],&weimi_para.step2_factor[6]);
    }
    strx = strstr(diskbuf,"八段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[14],&weimi_para.wei_cm_set[7],&weimi_para.wei_inch_set[7],
             &weimi_para.total_wei_count[15],&weimi_para.step1_factor[7],&weimi_para.step2_factor[7]);
    }
    strx = strstr(diskbuf,"九段");
    if(strx)
    {
      sscanf(strx,"%*[^,],%[^\n]",da);
      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[16],&weimi_para.wei_cm_set[8],&weimi_para.wei_inch_set[8],
             &weimi_para.total_wei_count[17],&weimi_para.step1_factor[8],&weimi_para.step2_factor[8]);
    }
//    strx = strstr(diskbuf,"十段");
//    if(strx)
//    {
//      sscanf(strx,"%*[^,],%[^\n]",da);
//      sscanf(da,"%d,%f,%f,%d,%d,%d",&weimi_para.total_wei_count[18],&weimi_para.wei_cm_set[9],&weimi_para.wei_inch_set[9],
//             &weimi_para.total_wei_count[19],&weimi_para.step1_factor[9],&weimi_para.step2_factor[9]);
//    }
  }
  
//  if(device_info.page_count_all >= 10)
//  {//去掉第一个文件，后面文件依次上移
//    u8 *file_read;
//    file_read = mymalloc(SRAMIN,1024);
//    W25QXX_Read(file_read,(u32)W25QXX_ADDR_JINGSHA + JINGSHA_SIZE,JINGSHA_SIZE * 9);//读出2-10文件的数据
//    W25QXX_Write(file_read,(u32)W25QXX_ADDR_JINGSHA,JINGSHA_SIZE * 9);//再写进1-9文件地址
//    W25QXX_Write((u8 *)&jingsha_file,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * 9),sizeof(jingsha_file));//写当前页面数据到文件10地址
//    
//    //产能数据保存
//    peiliao_file.complete_meter = 0;
//    peiliao_file.complete_work_time = 0;
//    W25QXX_Write((u8 *)&peiliao_file,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * 9,sizeof(peiliao_file));
//    
//    //纬密数据保存
//    W25QXX_Write((u8 *)&weimi_file,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * 9,sizeof(weimi_file));
//    myfree(SRAMIN,file_read);
//  }
//  else
//  {
//    W25QXX_Write((u8 *)&jingsha_file,(u32)(W25QXX_ADDR_JINGSHA + JINGSHA_SIZE * device_info.page_count_all),sizeof(jingsha_file));
//    //产能数据保存
//    peiliao_file.complete_meter = 0;
//    peiliao_file.complete_work_time = 0;
//    W25QXX_Write((u8 *)&peiliao_file,(u32)W25QXX_ADDR_PEILIAO + CHANNENG_SIZE * device_info.page_count_all,sizeof(peiliao_file));
//    
//    //纬密数据保存
//    W25QXX_Write((u8 *)&weimi_file,(u32)W25QXX_ADDR_WEIMI + WEIMI_SIZE * device_info.page_count_all,sizeof(weimi_file));
//    
//    device_info.page_count_all++;
//    W25QXX_Write((u8 *)&device_info,(u32)W25QXX_ADDR_INFO,sizeof(device_info));
//  }
}

u8* check_cmd(char *str1,char *str2)
{
  char *strx = 0;
  strx = strstr((const char*)str1,(const char*)str2);
  return (u8*)strx;
}