# -*- coding: utf-8 -*-
import re
import time

def getNews():
    f=open('train_data.txt','r')
    dt={}
    for i in range(116225):
        s=f.readline().strip()
        strl=s.split('\t')
        if(dt.has_key(int(strl[1]))):
            continue
        if(strl[5]!='NULL'):
            if(len(strl[5])<18):
                t=time.mktime(time.strptime(strl[5], '%Y年%m月%d日'))
            else:
                t=time.mktime(time.strptime(strl[5], '%Y年%m月%d日%H:%M'))
        else:
            t=0
        dt[int(strl[1])]=int(t)
    #print len(dt)
    f.close()
    fw=open('news.txt','w')
    newsIds=dt.keys()
    sorted_ids=sorted(newsIds)
    for newsid in sorted_ids:
        t=dt.get(newsid)
        ss=str(newsid)+'\t'+str(t)+'\n'
        fw.write(ss)
    fw.close()

def get_uid_nid():
    f=open('train_data.txt','r')
    fw=open('uid_nid.txt','w')
    fw_train=open('uid_nid_train.txt','w')
    fw_test=open('uid_nid_test.txt','w')
    prev_str='0000'
    for i in range(116225):
        s=f.readline()
        strl=s.split('\t')
        ss=strl[0]+'\t'+strl[1]+'\n'
        if(strl[0]!=prev_str):
            fw_test.write(ss)
        else:
            fw_train.write(ss)
        fw.write(ss)
        prev_str=strl[0]
    f.close()
    fw.close()
    fw_train.close()
    fw_test.close()


def remap_nid():
    f=open('news.txt','r')
    fw=open('news_id.txt','w')
    for i in range(6183):
        s=f.readline()
        strl=s.split('\t')
        ss=str(i)+'\t'+strl[0]+'\n'
        fw.write(ss)
    fw.close()
    f.close()
        
def remap_uid():
    f=open('uid_nid_test.txt','r')
    fw=open('user_id.txt','w')
    for i in range(10000):
        s=f.readline()
        strl=s.split('\t')
        ss=str(i)+'\t'+strl[0]+'\n'
        fw.write(ss)
    fw.close()
    f.close()  

def get_remapped_uid_nid ():
    uid_dict={}
    nid_dict={}
    f=open('user_id.txt','r')
    for i in range(10000):
        s=f.readline().strip()
        strl=s.split('\t')
        uid_dict[strl[1]]=strl[0]
    f.close()
    
    f=open('news_id.txt','r')
    for i in range(6183):
        s=f.readline().strip()
        strl=s.split('\t')
        nid_dict[strl[1]]=strl[0]
    f.close()
    f=open('uid_nid_train.txt','r')
    fw=open('remapped_uid_nid_train.txt','w')
    for i in range(106225):
        s=f.readline().strip()
        strl=s.split('\t')
        ss=uid_dict.get(strl[0])+'\t'+nid_dict.get(strl[1])+'\n'
        fw.write(ss)
    fw.close()
    f.close()
    
def get_user_last_news_time():
    news={}
    nid={}
    f_nid=open('news_id.txt','r')
    for i in range(6183):
        s=f_nid.readline().strip()
        strl=s.split('\t')
        nid[strl[0]]=strl[1]
    f_nid.close()
    f_news=open('news.txt','r')
    for i in range(6183):
        s=f_news.readline().strip()
        strl=s.split('\t')
        news[strl[0]]=strl[1]
    f_news.close()
    f=open('remapped_uid_nid.txt','r')
    fw=open('user_last_news_time.txt','w')
    prev_str='####'
    for i in range(116225):
        s=f.readline().strip()
        strl=s.split('\t')
        if(prev_str!=strl[0]):
            ss=strl[0]+'\t'+news.get(nid.get(strl[1]))+'\n'
            fw.write(ss)
        prev_str=strl[0]
    fw.close()
    f.close()

def remap_nid_time():
    news={}
    nid={}
    f_nid=open('news_id.txt','r')
    for i in range(6183):
        s=f_nid.readline().strip()
        strl=s.split('\t')
        nid[strl[1]]=strl[0]
    f_nid.close()
    f_news=open('news.txt','r')
    for i in range(6183):
        s=f_news.readline().strip()
        strl=s.split('\t')
        news[strl[0]]=strl[1]
    f_news.close()
    fw=open('remapped_nid_time.txt','w')
    for s in news.keys():
        ss=nid.get(s)+'\t'+news.get(s)+'\n'
        fw.write(ss)
    fw.close()

get_user_last_news_time()
    
    


