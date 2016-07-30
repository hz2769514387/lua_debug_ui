--[[
	������
		���ܣ�JSON����ģ��
		���ڣ�2016-04-13
		���ߣ�����
--]]

--[[
	���԰�����
	local _tb1,_Json
	
	_tb1 = CF_JsonStr2Table(' ' )
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"data" : "" }')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 = CF_JsonStr2Table('{"data":[],"retCode":"000000","retMsg":"ok"}')
	_Json = CF_JsonTable2Str(_tb1)
		
	_tb1 = CF_JsonStr2Table('{"data":null,"retCode":"499999","retMsg":"��֤���������"}')
	_Json = CF_JsonTable2Str(_tb1)
		
	_tb1 = CF_JsonStr2Table('{"data":44.75,"retCode":"499999","retMsg":true, "age":["bj",12,false]}')
	_Json = CF_JsonTable2Str(_tb1)	
	
	_tb1 = CF_JsonStr2Table('[null,2.56,"fred", {"first":true,"second":"venus","third":  "earth"}]')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"StringsList":["�Ͼ�","����","�е�","������","����"]}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"aPerson":{"address":"Beijing","id":1,"name":"ZhangSan"}}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"MapsList":[{"id":0,"address":"Address_0","name":"Name_0"},{"id":1,"address":"Address_1","name":"Name_1"},{"id":2,"address":"Address_2","name":"Name_2"}]}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"employees": [{ "firstName":"John" , "lastName":"Doe" },{ "firstName":"Anna" , "lastName":"Smith" },{ "firstName":"Peter" , "lastName":"Jones" }] }')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"data" : "���,����" ,"retCode":null,"retMsg":"��֤���������"}')
	_Json = CF_JsonTable2Str(_tb1)
	
	--�����ʧ�ܣ���Ϊkeyû��˫����
	_tb1 = CF_JsonStr2Table('{FBI:[{name:"rose",age:"25"},{name:"jack",age:"23"}],NBA:[{name:"tom",sex:"man"},{name:"jack",sex:"women"}]}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 = CF_JsonStr2Table('{"FBI":[{"name":"rose","age":"25"},{"name":"jack","age":"23"}],"NBA":[{"name":"tom","sex":"man"},{"name":"jack","sex":"women"}]}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 = CF_JsonStr2Table(
			'{\n' .. 
				'"employees": \n' ..
				'[\n' ..
					'{ "firstName":"John" , "lastName":"Doe" },\n' ..
					'{ "firstName":"Anna" , "lastName":"Smith" },\n' ..
					'{ "firstName":"Peter" , "lastName":"Jones" }\n' ..
				']\n' ..
			'}'
	)
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 = CF_JsonStr2Table(
	'{ ' ..
		'"code": 200, ' ..
		'"info": "success", ' ..
		'"list": [ ' ..
		'	{ ' ..
		'		"amount": 1, ' ..
		'		"code": "", ' ..
		'		"currency": 4, ' ..
		'		"id": 3, ' ..
		'		"name": "ֽƤ", ' ..
		'		"pic": "upload/recy/pic/baozhi520.png", ' ..
		'		"type": "ֽ��", ' ..
		'"barCode": "6930953013007",' ..
		'		"unit": "��"' ..
		'	}, ' ..
		'	{ ' ..
		'		"amount": 1, ' ..
		'		"code": "", ' ..
		'		"currency": 4, ' ..
		'		"id": 4, ' ..
		'		"name": "��ֽ", ' ..
		'		"pic": "upload/recy/pic/baozhi520.png", ' ..
		'		"type": "ֽ��", ' ..
		'"barCode": "6930953013007",' ..
		'' ..
		'		"unit": "��"' ..
		'	}' ..
		']' ..
	'}'
	)
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 = CF_JsonStr2Table(
			' 	{"����": "Ů������", ' .. 
			'	"С��" : ["Ϻ��1з��",{"name":"�����ֵ�Ϻ��", "skill":["�õĴ���","û�������"],"ս����":{"����":7,"������":"ս5��"}}],' .. 
			' 	"Age" : null, ' .. 
			'	"����" : {"name" : "��«��", "number":[2,4,6,"78"],"����":{"����":"Ů������"}}}' 
		
		)
	_Json = CF_JsonTable2Str(_tb1)
	
--]]


--[[
	�������ܣ���JSON�ַ���ת��Ϊtable��ʽ��
	���������
		_Str		JSON�ַ���������JSON�淶��KEY�����˫���ţ���VALUEΪ�ַ���ʱҲ�����˫���ţ�������ʹ�õ����š���{ "firstName":"John" , "lastName":null, "Age" = 4, "Marr" = false }
	���ز�����
		_Table		table��ʧ��Ϊnil������֮����������飬���������Ӧ��table�������ARRAYTYPE_CountsԪ�ر�ʾ�����е�Ԫ������
--]]
function CF_JsonStr2Table(_JsonStr)
	
	--�Ƴ�ǰ��Ŀհ��ַ�
	local _RealJson = JSON_TrimLR(_JsonStr)
	if nil == _RealJson then
		return nil
	end
	
	--�ж�json�ַ���������һ��json������һ��json����
	local _FirstChar = string.sub(_RealJson,1,1)
	local _EndChar = string.sub(_RealJson,-1,-1)
	
	if "[" == _FirstChar and "]" == _EndChar then
		_RealJson = string.sub(_RealJson ,2 , -2)
		return JSON_JsonArray2Table(_RealJson)
	elseif "{" == _FirstChar and "}" == _EndChar then
		_RealJson = string.sub(_RealJson ,2 , -2)
		return JSON_JsonObj2Table(_RealJson)
	else
		CF_WriteLog("ERROR", "CF_JsonStr2Tableʧ��,�����json�ַ������Ϸ���" .. _JsonStr)
		return nil
	end
end
	
	
--[[
	�������ܣ���table��ʾ��json����ת��ΪJSON�ַ���
	���������
		_Table		��ת����table
	���ز�����
		_JsonStr	ת�����json�ַ�����ʧ��Ϊnil
--]]
function CF_JsonTable2Str(_Table)
	if nil == _Table  then
		CF_WriteLog("ERROR", "CF_JsonTable2Strʧ�ܣ�_TableΪnil")
		return nil
	end
	local _JsonStr = ""
	
	local _Counts = _Table["ARRAYTYPE_Counts"] 
	if nil ~= _Counts then
		--����table����һ������
		_JsonStr = _JsonStr .. "["
		
		local _Index = 0
		while _Index  < _Counts do
			_Index = _Index + 1
			local _Value = _Table["" .. _Index]
			local _JsonValueStr = JSON_ValueObj2Str(_Value)
			if nil == _JsonValueStr then
				return nil
			end
			
			if _Index < _Counts then
				_JsonStr = _JsonStr .. _JsonValueStr .. ","
			else
				_JsonStr = _JsonStr .. _JsonValueStr
			end
		end
		
		_JsonStr = _JsonStr .. "]"
	else
		--tableΪjson����
		_JsonStr = "{"
		
		local _Key = nil
		local _Value = nil
		for  _Key, _Value in pairs(_Table) do	
			local _JsonValueStr = JSON_ValueObj2Str(_Value)
			if nil == _JsonValueStr then
				return nil
			end
			_JsonStr = _JsonStr .. ( "\"" .. _Key .. "\":" .. _JsonValueStr .. ",")
		end
		if "," == string.sub(_JsonStr,-1,-1) then
			_JsonStr = string.sub(_JsonStr,1,-2)
		end
		
		_JsonStr = _JsonStr .. "}"
	end
	
	return _JsonStr
end


	
	
---------------------------------------------------------------------
--						����Ϊ�ڲ���������ò�Ҫֱ��ʹ��			-
---------------------------------------------------------------------




--[[
	�������ܣ���JSONֵValueת��Ϊ�ַ���
	���������
		_ValueObj		JSONֵValue����
	���ز�����
		_JsonValueStr	�ַ�����JSON��ʽ
--]]
function JSON_ValueObj2Str(_ValueObj)

	local _JsonValueStr = ""
	local _ValueType = type(_ValueObj)
	if "string" == _ValueType then
		_JsonValueStr = "\"" .. _ValueObj .. "\""
	elseif  "number" == _ValueType then
		_JsonValueStr = "" .. _ValueObj
	elseif  "boolean" == _ValueType then
		if _ValueObj == true then
			_JsonValueStr = "true" 
		else
			_JsonValueStr = "false" 
		end
	elseif  "nil" == _ValueType then
		_JsonValueStr = "null"
	elseif  "table" == _ValueType then
		_JsonValueStr = CF_JsonTable2Str(_ValueObj)
	else
		CF_WriteLog("ERROR", "JSON_ValueObj2Strʧ�ܣ�_ValueObj����Ϊ" .. _ValueType)
		return nil
	end
	
	return _JsonValueStr
end
	
--[[
	�������ܣ���JSON�����ʾ���ַ���ת��Ϊtable��ʽ
	���������
		_Str		JSON�����ʾ���ַ�����
					��"firstName":"John" , "lastName":"Doe" 
	���ز�����
		_Table		table��ʧ��Ϊnil
--]]
function JSON_JsonObj2Table(_Str)
	--�Ƴ�ǰ��Ŀհ��ַ�
	local _RealJson = JSON_TrimLR(_Str)
	if nil == _RealJson then
		return nil
	end
	
	--�ж�_JsonStr�ǲ���һ���հ׵�json����
	if "" == _RealJson  then
		return {}
	end
	
	local  _ret, _key, _value, _left 
	
	local _Table = {}
	while _RealJson ~= nil do
		_ret, _key, _value, _left = JSON_GetFirstKVPair(_RealJson)
		if _ret == nil then
			return nil
		elseif _ret == 0 then
			--Valueֵ��һ��Json����
			local _ValueProcceed = JSON_JsonArray2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_Table[_key] = _ValueProcceed
		elseif _ret == 1 then
			--Valueֵ��һ��Json����
			local _ValueProcceed = JSON_JsonObj2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_Table[_key] = _ValueProcceed
		elseif _ret == 2 then
			--ValueֵΪ��ֵͨ
			_Table[_key] = _value
		end
		
		_RealJson = _left
	end
	
	return _Table
end

--[[
	�������ܣ���JSON�����ʾ���ַ���ת��Ϊtable��ʽ
	���������
		_Str		JSON�����ʾ���ַ�����
					��1,2,"fred", {"first":"mars","second":"venus","third","earth"}
	���ز�����
		_Table		table��ʧ��Ϊnil
--]]
function JSON_JsonArray2Table(_Str)
	--�Ƴ�ǰ��Ŀհ��ַ�
	local _RealJson = JSON_TrimLR(_Str)
	if nil == _RealJson then
		return nil
	end
	
	local _Table = {}
	local _ArrayCount = 0
	
	--�ж�_JsonStr�ǲ���һ���հ׵�json����
	if "" == _RealJson then
		_Table["ARRAYTYPE_Counts"] = _ArrayCount
		return  _Table
	end
	
	while _RealJson ~= nil  do
		
		local _ret, _value, _left =  JSON_GetFirstValue(_RealJson)
		
		if _ret == nil then
			return nil
		elseif _ret == 0 then
			--Valueֵ��һ��Json����
			local _ValueProcceed  = JSON_JsonArray2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_ArrayCount = _ArrayCount + 1
			_Table["" .. _ArrayCount] = _ValueProcceed
		elseif _ret == 1 then
			--Valueֵ��һ��Json����
			local _ValueProcceed = JSON_JsonObj2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_ArrayCount = _ArrayCount + 1
			_Table["" .. _ArrayCount] = _ValueProcceed
		elseif _ret == 2 then
			--ValueֵΪ��ֵͨ
			_ArrayCount = _ArrayCount + 1	
			_Table["" .. _ArrayCount] = _value
		end
		
		_RealJson = _left
	end
	 
	_Table["ARRAYTYPE_Counts"] = _ArrayCount
	return _Table
end

--[[
	�������ܣ����ַ����д�ͷ��ʼ��ȡ��һ��json��ֵ��
	���������
		_Str		json�ַ���
	���ز�����
		_ret		����ִ�н����nilʱΪʧ�ܣ�0 ֵΪ���� 1 ֵΪJSON���� 2 ֵΪ��ֵͨ 3 Ϊ��
		_key		��
		_value		ֵ
		_left		��ȡ��ֵ��֮��ʣ�µ��ַ���
--]]	
function JSON_GetFirstKVPair(_Str)
	--�Ƴ�ǰ��Ŀհ��ַ�
	local _RealKVPairLeft = JSON_TrimLR(_Str)
	if nil == _RealKVPairLeft then
		return nil
	end
	
	--�ж��ַ����Ƿ�Ϊ���ַ�����
	if "" == _RealKVPairLeft then
		return 3
	end
	
	--���������ַ���ͷ��ʼ�ĵ�һ��key
	local _key,_keyleft = string.match(_RealKVPairLeft, '^%"(.-)%"%s*:%s*(.*)$')
	if nil == _key  then
		CF_WriteLog("ERROR", "JSON_GetFirstKVPairʧ�ܣ����ַ��� " .. _RealKVPairLeft .. " ���Ҳ���KEY������KEY��ʽ���󣬼��һ��KEY�ǲ�����©��˫���ţ�")
		return nil
	end
	
	local _ret, _value, _left =  JSON_GetFirstValue(_keyleft)
	if nil == _ret then
		return nil
	end
	
	return _ret, _key, _value, _left
end

--[[
	�������ܣ����ַ����д�ͷ��ʼ��ȡ��һ��VALUE
	���������
		_Str		json�ַ���
	���ز�����
		_ret		����ִ�н����nilʱΪʧ�ܣ�0 ֵΪ���� 1 ֵΪJSON���� 2 ֵΪ��ֵͨ 3 Ϊ��
		_value		ֵ
		_left		��ȡ��ֵ��֮��ʣ�µ��ַ���
--]]	
function JSON_GetFirstValue(_Str)
	--�Ƴ�ǰ��Ŀհ��ַ�
	local _RealValueLeft = JSON_TrimLR(_Str)
	if nil == _RealValueLeft then
		return nil
	end
	
	--�ж��ַ����Ƿ�Ϊ���ַ�����
	if "" == _RealValueLeft then
		return 3
	end
	
	--VALUE����ʶ��
	local _FirstChar = string.sub(_RealValueLeft, 1 , 1 )
	
	local _ret = nil 
	local _value = nil
	local _left = nil 
	if "[" == _FirstChar then
		--Valueֵ��һ��Json����
		_ret = 0
		_value, _left = JSON_GetMarkValue(_RealValueLeft, "%[", "%]")
		if nil == _value then
			return nil
		end
	elseif "{" == _FirstChar then
		--Valueֵ��һ��Json����
		_ret = 1
		_value, _left = JSON_GetMarkValue(_RealValueLeft, "%{", "%}")
		if nil == _value then
			return nil
		end
	else
		--ValueֵΪ��ֵͨ
		_ret = 2	
		if "\"" == _FirstChar then
			--VALUEΪ�ַ�������
			_value, _left = string.match(_RealValueLeft,'^%"(.-)%"%s*,%s*(.*)$')
			if nil == _value then
				_left = nil
				_value = string.match(_RealValueLeft,'^%"(.-)%"$')
				if nil == _value then
					--������ֱ����Ϊ�ǿհ��ַ�����
					_value = ""
					--CF_WriteLog("ERROR", "JSON_GetFirstValueʧ�ܣ��ַ��� " .. _RealValueLeft .. " ʶ��Ϊ�ַ���������ƥ���ʶ�𲻵�ֵ")
					--return nil
				end
			end
		else
			--VALUEΪ���֡�null��true��false
			_value, _left = string.match(_RealValueLeft,'^(.-)%s*,%s*(.*)$')
			if nil == _value then
				_left = nil
				_value = _RealValueLeft

				if nil == _value then
					CF_WriteLog("ERROR", "JSON_GetFirstValueʧ�ܣ��ַ��� " .. _RealValueLeft .. " ʶ��Ϊ���ַ�����ֵͨ������ƥ���ʶ�𲻵�ֵ")
					return nil
				end
			end
			
			--ת��value
			if "null" == _value then
				_value = nil
			elseif "true" == _value then
				_value = true
			elseif "false" == _value then
				_value = false
			else
				_value = tonumber(_value)
				if nil == _value then
					CF_WriteLog("ERROR", "JSON_GetFirstValueʧ�ܣ��ַ��� " .. _RealValueLeft .. " ʶ��Ϊ���֣������޷��ɹ�ת��Ϊ����")
					return nil
				end
			end
		end
	end
	
	return _ret, _value, _left
end

--[[
	�������ܣ��Ƴ��ַ���ǰ��Ŀհ��ַ�
	���������
		_Str		�ַ���
	���ز�����
		_Str		�Ƴ��հ��ַ�����ַ�����ʧ��Ϊnil
--]]	
function JSON_TrimLR(_Str)
	if _Str == nil then
		CF_WriteLog("ERROR", "JSON_TrimLRʧ�ܣ��ַ���Ϊnil")
		return nil
	end
	
	--�Ƴ�ǰ��Ŀհ��ַ�
	local _StrLeft = string.match(_Str,'^%s*(.*)%s*$')
	if nil == _StrLeft  then
		CF_WriteLog("ERROR", "JSON_TrimLRʧ�ܣ��ַ��� " .. _Str .. " �Ƴ�ǰ��հ׺�ʣ�µ��ַ���Ϊnil")
		return nil
	end
	
	return _StrLeft
end

--[[
	�������ܣ�������һ��ƥ��ı�ǣ�����ƥ��õ��ַ�������
	���������
		_Str		������ַ���
		_MarkLeft	��Ƿ���ֻ���� %[ �� %{
		_MarkRight	��Ƿ���ֻ���� %] �� %}
	���ز�����
		_Str		ƥ��õ��ַ�����ʧ��Ϊnil
		_left		ƥ���֮��ʣ����ַ���
--]]	
function JSON_GetMarkValue(_Str, _MarkLeft, _MarkRight)
	--��һ���ַ��ض������ǣ��ӵڶ����ַ���ʼ
	local _StartPos = 2
	
	while true do
		--��ָ��λ�ÿ�ʼ�����ұ߱�ǣ�
		local _RightPos = string.find(_Str, _MarkRight, _StartPos)
		if nil == _RightPos then
			CF_WriteLog("ERROR", "JSON_GetMarkValueʧ�ܣ��ַ��� " .. _Str .. " ��ָ��λ��" .. _StartPos .. "��ʼ�����ұ߱�� " .. _MarkRight .. " ʧ��")
			return nil
		end
		
		local _MiddleStr = string.sub(_Str, _StartPos, _RightPos - 1)
		local _TmpPos = string.find(_MiddleStr, _MarkLeft, 1)
		if nil == _TmpPos then
			--����Ҳ�������ǰ��λ�ã�_RightPos������ƥ���λ��
			local _value = string.sub(_Str, 2,  _RightPos - 1)
			local _left = string.sub(_Str, _RightPos + 1)
			if nil == _value or nil == _left then
				return nil 
			end
			
			--������������ַ�����һ��λ��Ϊ���ţ��Ƴ�
			local _LeftStrRemoveFirstCharAndSpace = string.match(_left, '^%s*,%s*(.*)$')
			if nil == _LeftStrRemoveFirstCharAndSpace then
				return _value, _left
			else
				return _value, _LeftStrRemoveFirstCharAndSpace
			end
		else
			--����ҵ��ˣ�����λ�ü�����ǰ����
			_StartPos = _RightPos + 1
		end
	end
end