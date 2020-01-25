#!/usr/bin/python
#-*- coding: UTF-8 -*-
#**************************************************************
#
#          ��Ȩ���� (C), 2001-2011,��Ϊ�������޹�˾
#
#**************************************************************
#�ļ���    ��comm.py
#�汾��    ������
#��������  ��2013��09��09��
#��������  �������ӿ�
#ʹ�÷���  : 
#�޸���ʷ  ��
#1.����    ��2013��09��09��
#  �޸����ݣ������ļ�

def write_string_to_file(filepath,filename,string):
	"""
	���������������ļ�����д��ָ���ַ���
	������filepath �ļ�·��
	      filename �ļ���
		  string   �ַ���
	����ֵ��
	�쳣������
	�޸ļ�¼��1.����    : 2013��09��09��
				����    : l00167020
				�޸�����: ��������
	"""	
	filetowrite = filepath + "/" + filename
	f = file(filetowrite, "w")
	f.write(string)
	f.close()	

def GenerateBuildInfo(params_dict):
	"""
	�������������ɱ��빹���������Ϣ�ļ�����ð��ƽ̨ʹ��
	������params_dict����ֵ�
	����ֵ��
	�쳣������
	�޸ļ�¼��1.����    : 2013��09��09��
				����    : l00167020
				�޸�����: ��������
	"""	
	buildinfo=""
		
	for key in params_dict.keys():
#		if key != 'buildgroup' and\
		if "relversion" != key and \
		"transfer" != key and \
		"addname" != key and \
		"" != params_dict[key]:
			buildinfo += key + ":" + params_dict[key] + " "

	return buildinfo
	
def GenerateCmdArgFromDict(params_dict):
	"""
	�������������ֵ�ת��Ϊ�����в���
	������params_dict����ֵ�
	����ֵ��
	�쳣������
	�޸ļ�¼��1.����    : 2013��09��09��
				����    : l00167020
				�޸�����: ��������
	"""	
	CmdArgString=""
		
	for key in params_dict.keys():
		if "" != params_dict[key]:
			CmdArgString += key + "=" + params_dict[key] + " "
			
	return CmdArgString

def GetTargetFromArg(arg):
	"""
	���������� ��ȡcoverity,klocwork,fortifyЯ���ı���Ŀ��
	������arg coverity,klocwork,fortifyЯ���ı���Ŀ��
	����ֵ��
	�쳣������
	�޸ļ�¼��1.����    : 2013��10��31��
				����    : l00167020
				�޸�����: ��������
	"""	
	if not "true" == arg:
		return arg

	return ""