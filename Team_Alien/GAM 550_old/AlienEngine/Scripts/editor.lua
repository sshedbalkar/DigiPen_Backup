function Editor()
	--------------------------------------------------------------------------------------- init start 
	local activated = false;
	local screen_mode = 0;
	--------------------------------------------------------------------------------------- debug stuff
	local center0 = debug.NewBoxFrame();
	center0.x = 0.5; center0.y = 0.5; center0.z = 0;
	center0.sx = 8 / blackboard.screenw; center0.sy = 8 / blackboard.screenh;
	center0.cr = 0; center0.cg = 0; center0.cb = 0; center0.ca = 1.0;
	local center1 = debug.NewBox();
	center1.x = 0.5; center1.y = 0.5; center1.z = 0;
	center1.sx = 8 / blackboard.screenw; center1.sy = 8 / blackboard.screenh;
	center1.cr = 0.8; center1.cg = 0.7; center1.cb = 0.2; center1.ca = 0.3;
	--------------------------------------------------------------------------------------- editor components
	dofile( "scripts\\cmd.lua" );
	local thread_cmd = coroutine.create( Body );
	dofile( "scripts\\mousectrlcamera.lua" );
	local thread_dragcamera = coroutine.create( Body );
	dofile( "scripts\\dropmodels.lua" );
	local thread_dragndrop = coroutine.create( Body );
	dofile( "scripts\\groupSelection.lua" );
	local thread_group = coroutine.create( Body );
	--------------------------------------------------------------------------------------- msg process function 
	local MsgProc = function( msg )
		if ( msg.id == "vkdown" ) then
			if( msg.parama == C.VK_ESCAPE ) then
				activated = not activated;
				center0.show = activated;
				center1.show = activated;
				if ( activated ) then editor.ShowBars() else editor.HideBars() end
			elseif ( msg.parama == C.VK_TAB ) then
				screen_mode = screen_mode + 1;
				if ( screen_mode > 2 ) then screen_mode = screen_mode - 3; end
				if ( screen_mode == 0 ) then engine.Resolution( 800, 600 );
				elseif ( screen_mode == 1 ) then engine.Resolution( 1280, 720 );
				else engine.Resolution( 1920, 1080 ); end
			elseif ( msg.parama == C.VK_R ) then
				engine.LaunchLevel( "objects\\level_0.xml" );
			end
		elseif ( msg.id == "resolution" ) then
			center0.sx = 8 / blackboard.screenw; center0.sy = 8 / blackboard.screenh;
			center1.sx = 8 / blackboard.screenw; center1.sy = 8 / blackboard.screenh;
		end
	end
	SubscribeToMsg( MsgProc );
	--------------------------------------------------------------------------------------- init end
	while true do
		coroutine.yield();
	--	u.UpdateThread( thread_cmd, activated );
		u.UpdateThread( thread_dragcamera, activated );
		u.UpdateThread( thread_dragndrop, activated );
		u.UpdateThread( thread_group, activated );
	end
end
