script_list = {};
blackboard = { gt = 0.0, 
			   screenw = 800, screenh = 600, 
			   focused = 1, 
			   mx = 0, my = 0,
			   };

local coroutines = {};
local coroutine_ids = {};
local coroutine_objs = {};
local msg_subscriber = {};
local msg_queue = {};
local thread_editor;

function GameEntrance()
	dofile( "Scripts\\utility.lua" );
	engine.LaunchLevel( "Objects\\level_0.xml" );
end
------------------------- main update function -----------------------------
function Update( dt )
	--local mem, ters = gcinfo();
	--print("Memory: "..mem.."KB" );
	blackboard.dt = dt;
	blackboard.gt = blackboard.gt + dt;
	-------------------------- process message --------------------------
	for i,m in pairs( msg_queue ) do
		for j,func in pairs( msg_subscriber ) do
			func( m );
		end
		m.p = 1;
	end
	
	if ( thread_editor ~= nil ) then
		u.UpdateThread( thread_editor );
	else 
		dofile( "Scripts\\editor.lua" );
		thread_editor = coroutine.create( Editor );
	end
	-------------------------- update scripts ---------------------------
	for i,co in pairs( coroutines ) do
		local result, error = coroutine.resume( co );
		if ( result == false ) then
			print(error);
			coroutines[i] = nil;
			coroutine_ids[i] = nil;
			coroutine_objs[i] = nil;
		end
	end
	------------------------- sweep msg queue ----------------------------
	for i,j in pairs( msg_queue ) do if ( j.p == 1 ) then msg_queue[i] = nil end end
end
---------------------------------------------------------------------------------
function Unload()
	for i,m in pairs( msg_queue ) do
		msg_queue[i] = nil;
	end
	for j,func in pairs( msg_subscriber ) do
		msg_subscriber[j] = nil;
	end
	for i,co in pairs( coroutines ) do
		coroutines[i] = nil;
		coroutine_ids[i] = nil;
		coroutine_objs[i] = nil;
	end
	for i,scrpt in pairs( script_list ) do
		script_list[i] = nil;
	end
	thread_editor = nil;
end
---------------------------------------------------------------------------------
function ApplyScriptToObject( id, object )
	local func;
	if ( script_list[id] ~= nil ) then
		func = script_list[id];
	else
		if ( regpaths[id] == nil ) then
			print( "(lua)ApplyScriptToObject: script "..id.." not registered." );
			return;
		else
			dofile( regpaths[id] );
			func = Body;
			script_list[id] = Body;
			engine.RegisterScript( regpaths[id] );
		end
	end

	local co = coroutine.create( func );
	local result, msg = coroutine.resume( co, object );
	if ( result == false ) then 
		print(msg);
	else
		table.insert( coroutines, co );
		table.insert( coroutine_ids, id );
		table.insert( coroutine_objs, object );
	end
end
---------------------------------------------------------------------------------
function ReloadScript( id )
	local path = regpaths[id];
	dofile( path );
	script_list[id] = Body;

	for i,co in pairs( coroutines ) do
		if ( coroutine_ids[i] == id ) then
			coroutines[i] = nil;
			local co = coroutine.create( Body );
			coroutines[i] = co;
			coroutine.resume( co, coroutine_objs[i] );
		end
	end
end
---------------------------------------------------------------------------------
function PrintAllScriptNames()
	for i,sc in pairs( script_list ) do
		print(i);
	end
end
----------------------------** message related **---------------------------------
---------------------------------------------------------------------------------
function SubscribeToMsg( func )
	if ( func ~= nil ) then
		table.insert( msg_subscriber, func );
	end
end
---- function for communicating with the engine ------
function InputMessage( id, parama, paramb )
	--local msg = { id = id, parama = parama, paramb = paramb, p = 0 };
	table.insert( msg_queue, { id = id, parama = parama, paramb = paramb, p = 0 } );
	--AddMessageToQueue( msg );
end
