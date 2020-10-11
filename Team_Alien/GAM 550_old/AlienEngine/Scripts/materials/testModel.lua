function MaterialMain( graphics )
	----------------------------------------------------------------------------------------- renderer
	local renderer = graphics.NewRenderer();
	renderer:Sampler( "PS", gu.samplers.LinearClamp );
	----------------------------------------------------------------------------------------- variables
	local data = pointer.Chunk();
	local effect;
	----------------------------------------------------------------------------------------- cbuffer
	local frame_cb = graphics.NewCB(16 * 4);
	----------------------------------------------------------------------------------------- v/ibuffer
	local vformat_size;
	local vcount, vbuffer, vblite, icount, ibuffer, color, animated;
	graphics.InitData( data, 68 );
	data:SetI( 0 );
	data:Offset( data, 4 );
	data:SetF( 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 );
	data:Offset( data, -4 );
	local inst_vb = graphics.NewVB( 68, 0, 0, data );
	coroutine.yield();
	coroutine.yield();
	coroutine.yield();
	local prev_mesh = "";
	local prev_anim = "";

	local test_animctrl = pointer.AnimCtrl();
	graphics.TestAnimCtrl( test_animctrl );
	
	--end initialization
	--loop
	while true do
		local matrix, method = coroutine.yield();
		while ( method == gu.render_depth ) do matrix, method = coroutine.yield(); end
		if ( test_mesh ~= prev_mesh ) then
			vcount, vbuffer, vblite, icount, ibuffer, color, animated = graphics.LoadMesh( test_mesh );
			if ( vbuffer ~= nil ) then
				renderer:ClearVertexBuffer();
				renderer:ClearShaderResource( "PS" );
				renderer:ClearConstantBuffer( "VS" );
				renderer:ConstantBuffer( "VS", frame_cb );
				if ( animated == 1 ) then
					renderer:Effect( effects.amodel );
					renderer:VertexBuffer( vbuffer, 22 * 4 );
					renderer:ConstantBuffer( "VS", graphics.AnimationCB() );
				else
					renderer:Effect( effects.smodel );
					renderer:VertexBuffer( vbuffer, 14 * 4 );
				end
				renderer:VertexBuffer( inst_vb, 68 );
				renderer:IndexBuffer( ibuffer );
				renderer:ShaderResource( "PS", color );
			end
			prev_mesh = test_mesh;
		end
		if ( test_anim ~= prev_anim ) then
			local anim = graphics.LoadAnim( test_anim );
			test_animctrl:Switch(anim);
			prev_anim = test_anim;
		end
		if ( vbuffer ) then
			graphics.MapBuffer( data, frame_cb );
			data:AppendArray( matrix, 16 );
			graphics.UnmapBuffer();
		
			if ( animated == 1 ) then
				test_animctrl:Apply( blackboard.dt );
			end

			renderer:DrawIndexedInstanced( icount, 1 );
		end

	end
end