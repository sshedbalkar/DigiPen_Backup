function Body()

	local arrowX = debug.NewArrow();
	local arrowY = debug.NewArrow();
	local arrowZ = debug.NewArrow();

	arrowX.cr = 1; arrowX.ca = 1;
	arrowX.show = true;
	arrowY.cg = 1; arrowY.ca = 1;
	arrowY.show = true;
	arrowZ.cb = 1; arrowZ.ca = 1;
	arrowZ.show = true;

	local matrix = array.New(16);
	----------------------------------------------------------------------------------------------- function that calculate the center of the selection
	local goc = pointer.GOC();
	local transform = pointer.Transform();
	local data = pointer.Chunk();
	local x,y,z;
	local count;
	local CalculateCenter = function()
		goc:Transform( transform );
		transform:Position( data );
		local lx,ly,lz = data:GetF(3);
		x = x + lx; y = y + ly; z = z + lz;
		count = count + 1;
	end
	----------------------------------------------------------------------------------------------- function that check if a given position is in the range of an arrow
	local ArrowCheck = function( arrow, mx, my )
		local x0,y0,x1,y1 = arrow.x, arrow.y, arrow.sx, arrow.sy;
		local len = mmath.len2( x0 - x1, y0 - y1 );
		local dot = mmath.dot2( x1 - x0, y1 - y0, mx - x0, my - y0 );
		if ( dot < 0 ) then return false end
		dot = mmath.dot2( (x0 - x1)/len, (y0 - y1)/len, mx - x1, my - y1 );
		if ( dot < 0 ) then return false end
		local mlen = mmath.len2( mx - x1, my - y1 );
		local dis = mlen * mlen - dot * dot;
		dis = math.sqrt( dis );
		if ( dis < 0.01 ) then return true end
		return false;
	end
	----------------------------------------------------------------------------------------------- message processing
	local MsgProc = function( msg )
		if ( msg.id == "ldown" ) then
			local mx,my = blackboard.mx / blackboard.screenw, blackboard.my / blackboard.screenh;
			my = 1 - my;
			if ( ArrowCheck( arrowX, mx, my ) ) then 
				blackboard.xSelected=true
				print( "yesX" ) 
			else
				blackboard.xSelected=false
				print( "noX" ) 
			end
			if ( ArrowCheck( arrowY, mx, my ) ) then 
				blackboard.ySelected=true
				print( "yesY" ) 
			else
				blackboard.ySelected=false
				print( "noY" ) 
			end
			if ( ArrowCheck( arrowZ, mx, my ) ) then 
				blackboard.zSelected=true
				print( "yesZ" ) 
			else
				blackboard.zSelected=false
				print( "noZ" ) 
			end
		end
	end
	SubscribeToMsg( MsgProc );

	while true do
		local mgroup = coroutine.yield();

		levelinfo.camera:Matrices( matrix );
		mmath.Mat4Transpose( matrix );

		x = 0; y = 0; z = 0; count = 0;
		mgroup:ForEach( goc, CalculateCenter );
		x = x / count; y = y / count; z = z / count;
		local cx,cy = mmath.Mat4Transform( x, y, z, matrix );
		arrowX.sx,arrowX.sy = mmath.Mat4Transform( x + 1.5, y, z, matrix );
		arrowY.sx,arrowY.sy = mmath.Mat4Transform( x, y + 1.5, z, matrix );
		arrowZ.sx,arrowZ.sy = mmath.Mat4Transform( x, y, z + 1.5, matrix );
		arrowX.x = cx; arrowX.y = cy;
		arrowY.x = cx; arrowY.y = cy;
		arrowZ.x = cx; arrowZ.y = cy;
	end
end
