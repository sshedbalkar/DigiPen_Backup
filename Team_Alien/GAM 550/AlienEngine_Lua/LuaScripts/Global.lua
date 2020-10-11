------ Global varaibles -------
g_scripts			= {};		--table of all the scripts
g_coroutines		= {};		--table of all the coroutines
g_msg_subscribers	= {};		--table of all the msg subscribers
g_msg_queue			= {};		--table of all the msgs that need to be processed
g_blackboard		= {			--table of global variables
						dt = 0.0,		--frame time
						gt = 0.0,		--global time since the launch of the app
						mx = 0.0, my = 0.0		--mouse x/y position
						};
-------------------------------
print("Hello Global");
-------------------------------
---------------------------Applies script to a GOC--------------------------
function ApplyScriptToObject( id, object )
	local func;
	if ( g_scripts[id] ~= nil ) then
		func = g_scripts[id];
	else
		if ( m_regScripts[id] == nil ) then
			print( "(LUA)ApplyScriptToObject: script "..id.." not registered." );
			return;
		else
			dofile( m_regScripts[id] );
			func = Body;
			g_scripts[id] = Body;
		end
	end
	--
	local co = coroutine.create( func );
	local result, msg = coroutine.resume( co, object );
	if ( result == false ) then 
		print(msg);
	else
		g_coroutines[id] = { coroutine = co, obj = object };
	end
end
------------------------- main update function -----------------------------
function Update2( dt )
	--local mem, ters = gcinfo();
	--print("Memory: "..mem.."KB" );
	--g_blackboard.dt = dt;
	--g_blackboard.gt = g_blackboard.gt + dt;
	--[[
	-------------------------- process message --------------------------
	for i,m in pairs( g_msg_queue ) do
		--print(m.msg.MessageId);
		--for j,func in pairs( g_msg_subscribers ) do
			--func( m.msg );
		--end
		m.processed = 1;
	end
	
	-------------------------- update scripts ---------------------------
	
	for i, co in pairs( g_coroutines ) do
		local result, error = coroutine.resume( co["coroutine"] );
		if ( result == false ) then
			print(error);
			g_coroutines[i]["coroutine"] = nil;
			g_coroutines[i]["obj"] = nil;
			g_coroutines[i] = nil;
			
		end
	end
	
	
	------------------------- sweep msg queue ----------------------------
	for i,j in pairs( g_msg_queue ) do
		if ( j.processed == 1 ) then
			g_msg_queue[i] = nil;
		end
	end
	--]]
end
--
function Dummy1( v1 )
	print("in dummy");
end
-------------------------------------Adds a msg subscriber-------------------------------
function AddMsgSubscriber( func )
	if( func ~= nil ) then
		table.insert( g_msg_subscribers, func );
	end
end
------------------------------------Removes a msg subscriber-----------------------------
function RemoveMsgSubscriber( func )
	for i,j in pairs( g_msg_subscribers ) do0
		if ( j == func ) then
			g_msg_subscribers[i] = nil;
		end
	end
end
----------------------------------Adds a Lua msg to the msg list to be processed --------
function AddNewMessage( msg )
	--if(msg.MessageId == Message.MouseMove) then
		--print(msg.Var3);
	--end
	table.insert( g_msg_queue, {msg = msg, processed = 0 } );
end