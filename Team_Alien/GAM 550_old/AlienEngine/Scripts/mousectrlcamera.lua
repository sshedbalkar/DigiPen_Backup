function Body( object )
	-------------------------- init --------------------------
	blackboard.lhold = false;
	blackboard.rhold = false;
	blackboard.mhold = false;
	local mousex, mousey;
	local editor_on = false;
	local height = 0;
	-------------------------- message process ----------------
	local MsgProc = function( msg )
		if ( msg.id == "rdown" ) then
			blackboard.rhold = true;
		elseif ( msg.id == "rup" ) then
			blackboard.rhold = false;
		elseif ( msg.id == "ldown" ) then
			blackboard.lhold = true;
		elseif ( msg.id == "lup" ) then
			blackboard.lhold = false;
		elseif ( msg.id == "mdown" ) then
			blackboard.mhold = true;
		elseif ( msg.id == "mup" ) then
			blackboard.mhold = false;
		elseif ( msg.id == "mwheel" and editor_on ) then
			levelinfo.camera:Translate( 0, 0, msg.parama * 0.3 );
		end
	end
	SubscribeToMsg( MsgProc );
	coroutine.yield();
	mousex = blackboard.mx;
	mousey = blackboard.my;

	while true do
		editor_on = coroutine.yield();
		if ( editor_on ) then
			local dx = blackboard.mx - mousex;
			local dy = blackboard.my - mousey;
			height = height + dy * 0.01;
			if ( blackboard.rhold ) then
				levelinfo.camera:Rotate( -dx * 0.01, dy * 0.01, 0 );
			end
			if ( blackboard.mhold ) then
				levelinfo.camera:Translate( dx * 0.03, dy * 0.03, 0.0 );
			end
		else
			blackboard.rhold = false;
			blackboard.mhold = false;
		end
		mousex = blackboard.mx;
		mousey = blackboard.my;
	end
end
