--[[
	创建：
		功能：JSON解析模块
		日期：2016-04-13
		作者：黄哲
--]]

--[[
	测试案例：
	local _tb1,_Json
	
	_tb1 = CF_JsonStr2Table(' ' )
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"data" : "" }')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 = CF_JsonStr2Table('{"data":[],"retCode":"000000","retMsg":"ok"}')
	_Json = CF_JsonTable2Str(_tb1)
		
	_tb1 = CF_JsonStr2Table('{"data":null,"retCode":"499999","retMsg":"验证码输入错误"}')
	_Json = CF_JsonTable2Str(_tb1)
		
	_tb1 = CF_JsonStr2Table('{"data":44.75,"retCode":"499999","retMsg":true, "age":["bj",12,false]}')
	_Json = CF_JsonTable2Str(_tb1)	
	
	_tb1 = CF_JsonStr2Table('[null,2.56,"fred", {"first":true,"second":"venus","third":  "earth"}]')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"StringsList":["南京","杭州","承德","哈尔滨","重庆"]}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"aPerson":{"address":"Beijing","id":1,"name":"ZhangSan"}}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"MapsList":[{"id":0,"address":"Address_0","name":"Name_0"},{"id":1,"address":"Address_1","name":"Name_1"},{"id":2,"address":"Address_2","name":"Name_2"}]}')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"employees": [{ "firstName":"John" , "lastName":"Doe" },{ "firstName":"Anna" , "lastName":"Smith" },{ "firstName":"Peter" , "lastName":"Jones" }] }')
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 =CF_JsonStr2Table('{"data" : "你好,世界" ,"retCode":null,"retMsg":"验证码输入错误"}')
	_Json = CF_JsonTable2Str(_tb1)
	
	--这个会失败，因为key没有双引号
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
		'		"name": "纸皮", ' ..
		'		"pic": "upload/recy/pic/baozhi520.png", ' ..
		'		"type": "纸类", ' ..
		'"barCode": "6930953013007",' ..
		'		"unit": "斤"' ..
		'	}, ' ..
		'	{ ' ..
		'		"amount": 1, ' ..
		'		"code": "", ' ..
		'		"currency": 4, ' ..
		'		"id": 4, ' ..
		'		"name": "报纸", ' ..
		'		"pic": "upload/recy/pic/baozhi520.png", ' ..
		'		"type": "纸类", ' ..
		'"barCode": "6930953013007",' ..
		'' ..
		'		"unit": "斤"' ..
		'	}' ..
		']' ..
	'}'
	)
	_Json = CF_JsonTable2Str(_tb1)
	
	_tb1 = CF_JsonStr2Table(
			' 	{"姓名": "女王大人", ' .. 
			'	"小弟" : ["虾兵1蟹将",{"name":"有名字的虾米", "skill":["好的大王","没问题大王"],"战斗力":{"力量":7,"攻击力":"战5渣"}}],' .. 
			' 	"Age" : null, ' .. 
			'	"敌人" : {"name" : "葫芦娃", "number":[2,4,6,"78"],"敌人":{"名字":"女王大人"}}}' 
		
		)
	_Json = CF_JsonTable2Str(_tb1)
	
--]]


--[[
	函数功能：把JSON字符串转换为table形式。
	传入参数：
		_Str		JSON字符串，根据JSON规范，KEY必须带双引号，当VALUE为字符串时也必须带双引号，不允许使用单引号。如{ "firstName":"John" , "lastName":null, "Age" = 4, "Marr" = false }
	返回参数：
		_Table		table，失败为nil。解析之后如果有数组，会在数组对应的table里面添加ARRAYTYPE_Counts元素表示数组中的元素数量
--]]
function CF_JsonStr2Table(_JsonStr)
	
	--移除前后的空白字符
	local _RealJson = JSON_TrimLR(_JsonStr)
	if nil == _RealJson then
		return nil
	end
	
	--判断json字符串主体是一个json对象还是一个json数组
	local _FirstChar = string.sub(_RealJson,1,1)
	local _EndChar = string.sub(_RealJson,-1,-1)
	
	if "[" == _FirstChar and "]" == _EndChar then
		_RealJson = string.sub(_RealJson ,2 , -2)
		return JSON_JsonArray2Table(_RealJson)
	elseif "{" == _FirstChar and "}" == _EndChar then
		_RealJson = string.sub(_RealJson ,2 , -2)
		return JSON_JsonObj2Table(_RealJson)
	else
		CF_WriteLog("ERROR", "CF_JsonStr2Table失败,传入的json字符串不合法：" .. _JsonStr)
		return nil
	end
end
	
	
--[[
	函数功能：把table表示的json对象转换为JSON字符串
	传入参数：
		_Table		待转换的table
	返回参数：
		_JsonStr	转换后的json字符串，失败为nil
--]]
function CF_JsonTable2Str(_Table)
	if nil == _Table  then
		CF_WriteLog("ERROR", "CF_JsonTable2Str失败，_Table为nil")
		return nil
	end
	local _JsonStr = ""
	
	local _Counts = _Table["ARRAYTYPE_Counts"] 
	if nil ~= _Counts then
		--整个table就是一个数组
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
		--table为json对象
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
--						以下为内部函数，最好不要直接使用			-
---------------------------------------------------------------------




--[[
	函数功能：把JSON值Value转换为字符串
	传入参数：
		_ValueObj		JSON值Value部分
	返回参数：
		_JsonValueStr	字符串，JSON格式
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
		CF_WriteLog("ERROR", "JSON_ValueObj2Str失败，_ValueObj类型为" .. _ValueType)
		return nil
	end
	
	return _JsonValueStr
end
	
--[[
	函数功能：把JSON对象表示的字符串转换为table形式
	传入参数：
		_Str		JSON对象表示的字符串，
					如"firstName":"John" , "lastName":"Doe" 
	返回参数：
		_Table		table，失败为nil
--]]
function JSON_JsonObj2Table(_Str)
	--移除前后的空白字符
	local _RealJson = JSON_TrimLR(_Str)
	if nil == _RealJson then
		return nil
	end
	
	--判断_JsonStr是不是一个空白的json对象
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
			--Value值是一个Json数组
			local _ValueProcceed = JSON_JsonArray2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_Table[_key] = _ValueProcceed
		elseif _ret == 1 then
			--Value值是一个Json对象
			local _ValueProcceed = JSON_JsonObj2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_Table[_key] = _ValueProcceed
		elseif _ret == 2 then
			--Value值为普通值
			_Table[_key] = _value
		end
		
		_RealJson = _left
	end
	
	return _Table
end

--[[
	函数功能：把JSON数组表示的字符串转换为table形式
	传入参数：
		_Str		JSON数组表示的字符串，
					如1,2,"fred", {"first":"mars","second":"venus","third","earth"}
	返回参数：
		_Table		table，失败为nil
--]]
function JSON_JsonArray2Table(_Str)
	--移除前后的空白字符
	local _RealJson = JSON_TrimLR(_Str)
	if nil == _RealJson then
		return nil
	end
	
	local _Table = {}
	local _ArrayCount = 0
	
	--判断_JsonStr是不是一个空白的json数组
	if "" == _RealJson then
		_Table["ARRAYTYPE_Counts"] = _ArrayCount
		return  _Table
	end
	
	while _RealJson ~= nil  do
		
		local _ret, _value, _left =  JSON_GetFirstValue(_RealJson)
		
		if _ret == nil then
			return nil
		elseif _ret == 0 then
			--Value值是一个Json数组
			local _ValueProcceed  = JSON_JsonArray2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_ArrayCount = _ArrayCount + 1
			_Table["" .. _ArrayCount] = _ValueProcceed
		elseif _ret == 1 then
			--Value值是一个Json对象
			local _ValueProcceed = JSON_JsonObj2Table(_value)
			if nil == _ValueProcceed then
				return nil
			end
			_ArrayCount = _ArrayCount + 1
			_Table["" .. _ArrayCount] = _ValueProcceed
		elseif _ret == 2 then
			--Value值为普通值
			_ArrayCount = _ArrayCount + 1	
			_Table["" .. _ArrayCount] = _value
		end
		
		_RealJson = _left
	end
	 
	_Table["ARRAYTYPE_Counts"] = _ArrayCount
	return _Table
end

--[[
	函数功能：从字符串中从头开始获取第一个json键值对
	传入参数：
		_Str		json字符串
	返回参数：
		_ret		函数执行结果，nil时为失败，0 值为数组 1 值为JSON对象 2 值为普通值 3 为空
		_key		键
		_value		值
		_left		获取键值对之后剩下的字符串
--]]	
function JSON_GetFirstKVPair(_Str)
	--移除前后的空白字符
	local _RealKVPairLeft = JSON_TrimLR(_Str)
	if nil == _RealKVPairLeft then
		return nil
	end
	
	--判断字符串是否为空字符串了
	if "" == _RealKVPairLeft then
		return 3
	end
	
	--解析出从字符串头开始的第一个key
	local _key,_keyleft = string.match(_RealKVPairLeft, '^%"(.-)%"%s*:%s*(.*)$')
	if nil == _key  then
		CF_WriteLog("ERROR", "JSON_GetFirstKVPair失败，从字符串 " .. _RealKVPairLeft .. " 查找不到KEY，可能KEY格式错误，检查一下KEY是不是遗漏了双引号？")
		return nil
	end
	
	local _ret, _value, _left =  JSON_GetFirstValue(_keyleft)
	if nil == _ret then
		return nil
	end
	
	return _ret, _key, _value, _left
end

--[[
	函数功能：从字符串中从头开始获取第一个VALUE
	传入参数：
		_Str		json字符串
	返回参数：
		_ret		函数执行结果，nil时为失败，0 值为数组 1 值为JSON对象 2 值为普通值 3 为空
		_value		值
		_left		获取键值对之后剩下的字符串
--]]	
function JSON_GetFirstValue(_Str)
	--移除前后的空白字符
	local _RealValueLeft = JSON_TrimLR(_Str)
	if nil == _RealValueLeft then
		return nil
	end
	
	--判断字符串是否为空字符串了
	if "" == _RealValueLeft then
		return 3
	end
	
	--VALUE类型识别
	local _FirstChar = string.sub(_RealValueLeft, 1 , 1 )
	
	local _ret = nil 
	local _value = nil
	local _left = nil 
	if "[" == _FirstChar then
		--Value值是一个Json数组
		_ret = 0
		_value, _left = JSON_GetMarkValue(_RealValueLeft, "%[", "%]")
		if nil == _value then
			return nil
		end
	elseif "{" == _FirstChar then
		--Value值是一个Json对象
		_ret = 1
		_value, _left = JSON_GetMarkValue(_RealValueLeft, "%{", "%}")
		if nil == _value then
			return nil
		end
	else
		--Value值为普通值
		_ret = 2	
		if "\"" == _FirstChar then
			--VALUE为字符串类型
			_value, _left = string.match(_RealValueLeft,'^%"(.-)%"%s*,%s*(.*)$')
			if nil == _value then
				_left = nil
				_value = string.match(_RealValueLeft,'^%"(.-)%"$')
				if nil == _value then
					--这里先直接认为是空白字符串吧
					_value = ""
					--CF_WriteLog("ERROR", "JSON_GetFirstValue失败，字符串 " .. _RealValueLeft .. " 识别为字符串，但是匹配后识别不到值")
					--return nil
				end
			end
		else
			--VALUE为数字、null、true或false
			_value, _left = string.match(_RealValueLeft,'^(.-)%s*,%s*(.*)$')
			if nil == _value then
				_left = nil
				_value = _RealValueLeft

				if nil == _value then
					CF_WriteLog("ERROR", "JSON_GetFirstValue失败，字符串 " .. _RealValueLeft .. " 识别为非字符串普通值，但是匹配后识别不到值")
					return nil
				end
			end
			
			--转换value
			if "null" == _value then
				_value = nil
			elseif "true" == _value then
				_value = true
			elseif "false" == _value then
				_value = false
			else
				_value = tonumber(_value)
				if nil == _value then
					CF_WriteLog("ERROR", "JSON_GetFirstValue失败，字符串 " .. _RealValueLeft .. " 识别为数字，但是无法成功转换为数字")
					return nil
				end
			end
		end
	end
	
	return _ret, _value, _left
end

--[[
	函数功能：移除字符串前后的空白字符
	传入参数：
		_Str		字符串
	返回参数：
		_Str		移除空白字符后的字符串，失败为nil
--]]	
function JSON_TrimLR(_Str)
	if _Str == nil then
		CF_WriteLog("ERROR", "JSON_TrimLR失败，字符串为nil")
		return nil
	end
	
	--移除前后的空白字符
	local _StrLeft = string.match(_Str,'^%s*(.*)%s*$')
	if nil == _StrLeft  then
		CF_WriteLog("ERROR", "JSON_TrimLR失败，字符串 " .. _Str .. " 移除前后空白后剩下的字符串为nil")
		return nil
	end
	
	return _StrLeft
end

--[[
	函数功能：查找下一个匹配的标记，并将匹配好的字符串返回
	传入参数：
		_Str		带标记字符串
		_MarkLeft	标记符，只允许 %[ 或 %{
		_MarkRight	标记符，只允许 %] 或 %}
	返回参数：
		_Str		匹配好的字符串，失败为nil
		_left		匹配好之后剩余的字符串
--]]	
function JSON_GetMarkValue(_Str, _MarkLeft, _MarkRight)
	--第一个字符必定是左标记，从第二个字符开始
	local _StartPos = 2
	
	while true do
		--从指定位置开始查找右边标记，
		local _RightPos = string.find(_Str, _MarkRight, _StartPos)
		if nil == _RightPos then
			CF_WriteLog("ERROR", "JSON_GetMarkValue失败，字符串 " .. _Str .. " 从指定位置" .. _StartPos .. "开始查找右边标记 " .. _MarkRight .. " 失败")
			return nil
		end
		
		local _MiddleStr = string.sub(_Str, _StartPos, _RightPos - 1)
		local _TmpPos = string.find(_MiddleStr, _MarkLeft, 1)
		if nil == _TmpPos then
			--如果找不到，则当前的位置（_RightPos）即是匹配的位置
			local _value = string.sub(_Str, 2,  _RightPos - 1)
			local _left = string.sub(_Str, _RightPos + 1)
			if nil == _value or nil == _left then
				return nil 
			end
			
			--如果留下来的字符串第一个位置为逗号，移除
			local _LeftStrRemoveFirstCharAndSpace = string.match(_left, '^%s*,%s*(.*)$')
			if nil == _LeftStrRemoveFirstCharAndSpace then
				return _value, _left
			else
				return _value, _LeftStrRemoveFirstCharAndSpace
			end
		else
			--如果找到了，查找位置继续向前推移
			_StartPos = _RightPos + 1
		end
	end
end