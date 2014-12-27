def rank(u,idx):
    x=u[idx]
    cnt=0
    for y in u:
        if(y>=x):
            cnt+=1
    return cnt

def topk_percent(k):
    uid_nid=[]
    f=open('remapped_uid_nid_test.txt','r')
    for i in range(10000):
        s=f.readline().strip()
        strl=s.split('\t')
        uid_nid.append(strl[1])
    f.close()
    f=open('matrix_knn_train.txt','r')
    cnt=0
    for i in range(10000):
        s=f.readline().strip()
        strl=s.split(' ')
        intl=[float(t) for t in strl]
        r=rank(intl,int(uid_nid[i]))
        if(r<k):
            cnt+=1
    return cnt

#print topk_percent(100)

def get_top_k_item(u,k):
    rt=[-1 for i in range(k)]
    maxx=0
    idx=0
    for i in range(k):
        for j in range(len(u)):
            if(u[j]>maxx and j not in rt):
                maxx=u[j]
                idx=j
        rt[i]=idx
        maxx=0
    return rt

def get_top_recommendation(k):
    f=open('user_last_news_time_train.txt','r')
    ul=[]
    for i in range(10000):
        s=f.readline().strip()
        strl=s.split('\t')
        ul.append(int(strl[1]))
    f.close()
    f=open('remapped_nid_time.txt','r')
    nt={}
    for i in range(6183):
        s=f.readline().strip()
        strl=s.split('\t')
        nt[int(strl[0])]=int(strl[1])
    f.close()
    
    f=open('matrix_knn_train.txt','r')
    rt=[-1 for i in range(10000)]
    for i in range(10000):
        s=f.readline().strip()
        strl=s.split(' ')
        intl=[float(t) for t in strl]
        topk=get_top_k_item(intl,k)
        rt[i]=topk[0]
        utm=ul[i]
        if(utm==0):
            continue
        for j in topk:
            tm=nt.get(j)
            if(tm!=0 and abs(tm-utm)<60000):
                rt[i]=j
                break
    f.close()
    return rt

def test():
    rslt=get_top_recommendation(100)
    f=open('remapped_uid_nid_test.txt','r')
    cnt=0
    for i in range(10000):
        s=f.readline().strip()
        strl=s.split('\t')
        if(rslt[int(strl[0])]==int(strl[1])):
            cnt+=1
    f.close()
    return cnt
print test()


def output_result():
    rslt=get_top_recommendation(100)
    f=open('user_id.txt','r')
    uid=[]
    for i in range(10000):
        s=f.readline().strip()
        strl=s.split('\t')
        uid.append(strl[1])
    f.close()

    f=open('news_id.txt','r')
    nid=[]
    for i in range(6183):
        s=f.readline().strip()
        strl=s.split('\t')
        nid.append(strl[1])
    f.close()
    
    f=open('result4.txt','w')
    for i in range(10000):
        ss=uid[i]+','+nid[rslt[i]]+'\n'
        f.write(ss)
    f.close()

#output_result()           
            
    
    
        
