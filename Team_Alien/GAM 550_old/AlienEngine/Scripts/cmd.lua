local cmd_list = {};
------------------------------------------------
local function CmdReloadEffect( param )
	local name = mstring.Parameter( param, "s" );
	local index = Graphics.GetEffect( name );
	Graphics.ReloadEffect( index );
end
------------------------------------------------
local function CmdReloadScript( param )
	local name = mstring.Parameter( param, "s" );
	ReloadScript( name );
end
------------------------------------------------
function AddCmd( name, func )
	cmd_list[name] = func;
end
------------------------------------------------
function Body()
	------------------------- init start -----------------------------
	local cur_line = "";
	local prev_line = "";
	local activated = false;
	local transform = pointer.Transform();
	local pos = pointer.Chunk();
	local hidey = -0.1;
	local showy = 0.0;
	
	local max_length = 40;
	local show_line, object = engine.DebugText( 0.01, hidey, max_length );
	object:Transform(transform);
	transform:Position(pos);	

	pos:SetF( 0.1, hidey, 0.0 );

	show_line:SetS(">", max_length );
	AddCmd( "rlfx", CmdReloadEffect );
	AddCmd( "rlscript", CmdReloadScript );
	----------------------- show&hide function -----------------------
	local Show = function()
		local x,y = pos:GetF(2);
		while ( y < showy ) do
			y = y + blackboard.dt * 0.5;
			pos:SetF(x,y);
			coroutine.yield();
		end
	end
	local Hide = function()
		local x,y = pos:GetF(2);
		while ( y > hidey ) do
			y = y - blackboard.dt * 0.5;
			pos:SetF(x,y);
			coroutine.yield();
		end
	end
	---------------------------- init end ----------------------------
	coroutine.yield();
	------------------- process message function ---------------------
	local MsgProc = function( msg )
		-------------------- character key input --------------------
		if ( msg.id == "ckdown" ) then
			if ( cur_line:len() < max_length ) then cur_line = cur_line..string.format( "%c", msg.parama ); end
		------------------- deal with virtual keys -------------------
		elseif ( msg.id == "vkdown" ) then
			--------------------- backspace -------------------------
			if ( msg.parama == C.VK_BACK ) then
				if ( cur_line:len() > 1 ) then cur_line = cur_line:sub( 1, -2 ); end
			--------------------- return ----------------------------
			elseif ( msg.parama == C.VK_RETURN ) then
				local div = cur_line:find( " " );
				if ( div ~= nil ) then
					local funcname = cur_line:sub( 2, div - 1 );
					local func = cmd_list[funcname];
					if ( func ~= nil ) then func( cur_line:sub( div + 1, -1 ) ) end
				end
				prev_line = cur_line;
				cur_line = ">";
			-------------- up( resume last command ) ----------------
			elseif ( msg.parama == C.VK_UP ) then
				cur_line = prev_line;
			end
		end
	end
	SubscribeToMsg( MsgProc );
	------------------------ main loop ------------------------------
	while true do
		local editor_on = coroutine.yield();
		------------ check if the editor is launched ----------------
		if ( activated and not editor_on ) then
			Hide();
		elseif ( not activated and editor_on ) then
			cur_line = ">";
			show_line:SetS( cur_line, max_length );
			Show();
		end
		activated = editor_on;
		if ( activated ) then
			show_line:SetS( cur_line, max_length );
		end
	end
end
