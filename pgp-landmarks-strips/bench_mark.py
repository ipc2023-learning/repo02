
import threading
# 导入进程模块
import multiprocessing
import subprocess

import os
import psutil
import time
import resource
from signal import SIGKILL
limited=8*1024*1024*1024

def wirte(context):
    with open("memory_error.txt","a+") as variable_name:
        variable_name.write(context+" Memory budget exceeded\n")

def checkMemory(pid,cmd):
    while True:
        # try:
        #     process = psutil.Process(pid)
        # except:
        #     break
        try:
            # memoryUsage = process.memory_full_info().rss #in bytes
            current_process = psutil.Process(pid)
            # time=current_process.cpu_times()
            mem = current_process.memory_info().rss
            for child in current_process.children(recursive=True):
                try:
                    if child.name()=="sh" or child.name()=="time":
                        continue
                    mem += child.memory_info().rss# if memoryUsage > limited:
                except:
                    pass
        except:
            break
        if mem>limited:
            print(cmd,"memory exceed")
            break
    try:
        for child in current_process.children(recursive=True):
            if child.name()=="time":
                continue
            print("memory:",child.pid,child.name())
            child.kill()
        print("############################")
        # os.kill(pid,SIGKILL)
    except:
        pass

def cmd(command,result_dict,file):

    restriction='/usr/bin/time -f "time result\ncmd:%C\nreal %es\nmemory:%MKB \n" $@ 2>> experiments/' + file +".out "
    result_dict[file]=dict()
    command=restriction+command
    # subp = subprocess.Popen(command, stdout=subprocess.PIPE,stderr=subprocess.PIPE)
    subp = subprocess.Popen(command,shell=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE,encoding="utf-8")
    # subp = subprocess.Popen(command,shell=True,encoding="utf-8")
    # subp = subprocess.run(args=command.split(" "),shell=False,stdout=subprocess.PIPE,stderr=subprocess.PIPE,encoding="utf-8")
    threading.Thread(name="Memory Regulator", target=checkMemory,args=(subp.pid,command)).start()

    try:
        subp.wait(3600)
    except:
        try:
            current_process = psutil.Process(subp.pid)
            for child in current_process.children(recursive=True):
                if child.name()=="time":
                    continue
                print("time:",child.pid,child.name())
                child.kill()
        except:
            pass
    if subp.poll() == 0:
        print("find!!!!!!!")
        for line in (subp.communicate()[0]).split("\n"):
            print(line)
        subp.terminate()
    else:
        print(cmd,"memory")
        subp.terminate()

if __name__ == "__main__":
    result_dict = dict()
    pool = multiprocessing.Pool(processes=7)
    item_list = ['processes1', 'processes2', 'processes3', 'processes4','processes5' ,'processes6','processes7']
    # item_list = ['processes1', 'processes2' ]

    path="./scripts/check_line_strips.sh"
    f = open(path)
    iter_f = iter(f)
    for line in iter_f:
        if "#" in line:
            continue
        print(line)
        parameter=line.rstrip('\n')
        domain_name_list=parameter.split("/")
        record_name=[]
        for i in range(len(domain_name_list)-1,-1,-1):
            if domain_name_list[i]=="synthesis":
                break
            if " " in domain_name_list[i]:
                h_list=domain_name_list[i].split(" ")
                for j in range(len(h_list)-1,-1,-1):
                    if h_list[j]=="":
                        continue
                    record_name.append(h_list[j])
            if " " not in domain_name_list[i]:
                record_name.append(domain_name_list[i])
        record_name_memory="_".join(record_name[::-1])
        print(parameter)
        pool.apply_async(cmd,args=(parameter,result_dict,record_name_memory))
    pool.close()
    pool.join()