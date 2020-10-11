function Body()
	local mgroup = group.New();
	---------------------------------------------------------------------------------------- threads
	dofile( "scripts\\transformation.lua" );
	local thread_transformation = u.CreateThread( Body );

	local lhold = false;
	---------------------------------------------------------------------------------------- box of selection
	local x0, y0 = 0, 0;
	local boxframe = debug.NewBoxFrame();
	boxframe.cr = 0; boxframe.cg = 0; boxframe.cb = 0; boxframe.ca = 1;
	local boxsolid = debug.NewBox();
	boxsolid.cr = 0.1; boxsolid.cg = 0.4; boxsolid.cb = 1; boxsolid.ca = 0.3;
	---------------------------------------------------------------------------------------- 
	local diff=0;
	local mousepos=0
	local previous =0
	local startX = false
	local startY = false
	local startZ = false
	local editor_on;
	local goc = pointer.GOC();
	local gfxcomp = pointer.GfxObj();
	---------------------------------------------------------------------------------------- functions that deals with highlighting
	local Temp = function()
		goc:GfxObj( gfxcomp );
		gfxcomp:Flag( render_flags.RENDER_FLAG_HIGHLIGHTED );
	end
	local Temp2 = function()
		goc:GfxObj( gfxcomp );
		gfxcomp:Flag( -render_flags.RENDER_FLAG_HIGHLIGHTED );
	end
	---------------------------------------------------------------------------------------- message processing
	local MsgProc = function( msg )
		if ( editor_on == false ) then return end
		if ( msg.id == "lup" ) then
			local x1 = blackboard.mx / blackboard.screenw;
			local y1 = 1 - blackboard.my / blackboard.screenh;
			local xa,xb,ya,yb;
			if ( x0 < x1 ) then xa = x0; xb = x1; else xa = x1; xb = x0; end
			if ( y0 < y1 ) then ya = y0; yb = y1; else ya = y1; yb = y0; end
			mgroup:ForEach( goc, Temp2 );
			mgroup:Clear();
			mgroup:Select( xa, ya, xb, yb, levelinfo.camera );
			mgroup:ForEach( goc, Temp );
			mgroup:GetFirst(goc);
			editor.GetObject(goc);
		
			boxframe.show = false;
			boxsolid.show = false;
			lhold = false;
		elseif ( msg.id == "ldown" ) then
			x0 = blackboard.mx / blackboard.screenw;
			y0 = 1 - blackboard.my / blackboard.screenh;
			boxframe.show = true;
			boxsolid.show = true;
			lhold = true;
		--elseif ( msg.id == "vkdown"  ) then
			--------------------- backspace -------------------------
			
		end
	end
	SubscribeToMsg( MsgProc );
	---------------------------------------------------------------------------------------- loop
	while true do
		editor_on = coroutine.yield();
		u.UpdateThread( thread_transformation, mgroup );
		if ( editor_on == false ) then lhold = false end
		if ( lhold ) then
			local xa,xb,ya,yb;
			xa = x0;-- * blackboard.screenw;
			ya = y0;-- * blackboard.screenh;
			xb = blackboard.mx / blackboard.screenw;
			yb = 1 - blackboard.my / blackboard.screenh;
			boxframe.x = ( xa + xb ) * 0.5;
			boxframe.y = ( ya + yb ) * 0.5;
			boxframe.z = 0;
			boxframe.sx = math.abs( xa - xb );
			boxframe.sy = math.abs( ya - yb );
			boxsolid.x = boxframe.x;
			boxsolid.y = boxframe.y;
			boxsolid.z = boxframe.z;
			boxsolid.sx = boxframe.sx;
			boxsolid.sy = boxframe.sy;
		end


		-- have to move this code somewhere else
		if(blackboard.lhold==true) then
			if ( blackboard.xSelected==true ) then 
				mousepos=blackboard.mx
				if(startX==false) then
					previous = mousepos
					startX=true
				end
					
				if (previous ~= mousepos) then
					diff = mousepos-previous
					mgroup:MoveAll(diff/50,0.0,0.0,Temp );
					previous = mousepos
				end
			else
				startX=false
			end 
					
					
			if ( blackboard.ySelected==true ) then 
				mousepos=blackboard.my
				if(startY==false) then
					previous = mousepos
					startY=true
				end
					
				if (previous ~= mousepos) then
					diff = mousepos-previous
					mgroup:MoveAll(0.0,-diff/50,0.0,Temp );
					previous = mousepos
				end
			else
				startY=false
			end

			if ( blackboard.zSelected==true ) then 
				mousepos=blackboard.my
				if(startZ==false) then
					previous = mousepos
					startZ=true
				end
					
				if (previous ~= mousepos) then
					diff = mousepos-previous
					mgroup:MoveAll(0.0,0.0,-diff/50,Temp );
					previous = mousepos
				end
			else
				startZ=false
			end
		end

	end

end