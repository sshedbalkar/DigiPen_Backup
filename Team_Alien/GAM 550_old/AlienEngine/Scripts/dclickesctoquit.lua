function Body()
	coroutine.yield();

	local StateIdle, StateClicked;
	local clicked = false;
	local MsgProc = function( msg )
		if ( msg.id == "vkup" and msg.parama == C.VK_ESCAPE ) then clicked = true; end
	end
	SubscribeToMsg( MsgProc );

	StateIdle = function()
		while ( not clicked ) do
			coroutine.yield();
		end
		clicked = false;
		StateClicked();
	end

	StateClicked = function()
		local et = 0.0;
		while ( et < 0.2 ) do
			if ( clicked ) then engine.Shutdown(); end
			et = et + blackboard.dt;
			coroutine.yield();
		end
		StateIdle();
	end

	StateIdle();
	
end