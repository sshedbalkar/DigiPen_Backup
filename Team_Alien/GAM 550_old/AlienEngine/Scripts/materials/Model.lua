function MaterialMain( graphics, id )
	--coroutine.yield();
	----------------------------------------------------------------------------------- renderers
	local depth_renderer = graphics.NewRenderer();
	local color_renderer = graphics.NewRenderer();
	----------------------------------------------------------------------------------- register material
	local mat = graphics.NewMaterial( id );
	local effect;
	local effect_depth;
	----------------------------------------------------------------------------------- cbuffer
	local frame_cb = graphics.NewCB(16 * 4);
	depth_renderer:ConstantBuffer( "VS", frame_cb );
	color_renderer:ConstantBuffer( "VS", frame_cb );
	----------------------------------------------------------------------------------- instance variables
	local max_instances = 128;
	local num_insts = 0;
	local inst_vb;
	local mat_world = array.New(16);
	----------------------------------------------------------------------------------- v/ibuffer
	local vformat_size, vformat_size_depth;
	local vcount, vbuffer, vblite, icount, ibuffer, color, animated = graphics.LoadMesh( id );
	if ( animated == 1 ) then
		effect = effects.amodel;
		effect_depth = effects.amodeldepth;
		vformat_size = 22 * 4;
		vformat_size_depth = 11 * 4;
		inst_vb = graphics.NewVB( 68, 1, 0 );
		depth_renderer:ConstantBuffer( "VS", graphics.AnimationCB() );
		color_renderer:ConstantBuffer( "VS", graphics.AnimationCB() );
	else
		effect = effects.smodel;
		effect_depth = effects.smodeldepth;
		vformat_size = 14 * 4;
		vformat_size_depth = 3 * 4;
		inst_vb = graphics.NewVB( 68 * max_instances, 1, 0 );
	end
	depth_renderer:Effect( effect_depth );
	depth_renderer:VertexBuffer( vblite, vformat_size_depth );
	depth_renderer:VertexBuffer( inst_vb, 68 );
	depth_renderer:IndexBuffer( ibuffer );
	color_renderer:Effect( effect );
	color_renderer:VertexBuffer( vbuffer, vformat_size );
	color_renderer:VertexBuffer( inst_vb, 68 );
	color_renderer:IndexBuffer( ibuffer );
	color_renderer:ShaderResource( "PS", color );
	color_renderer:Sampler( "PS", gu.samplers.LinearClamp );

	----------------------------------------------------------------------------------- pointers 
	local data = pointer.Chunk();
	local transform = pointer.Transform();
	local go = pointer.GfxObj();
	local animctrl = pointer.AnimCtrl();
	----------------------------------------------------------------------------------- variables 
	local method = 0;
	----------------------------------------------------------------------------------- per object update function 
	local InstFunc;
	local InstFuncDepth;

	local temp = 0.0;

	local ttt = 0;
	----------------------------------------------------------------------------------- functions for updating animated models 
	if ( animated == 1 ) then
	------------------------------------------------------------- normal
		InstFunc = function()
			go:Transform(transform);
			go:AnimCtrl( animctrl );
			transform:BuildMatrix( mat_world );
			graphics.MapBuffer( data, inst_vb );
			data:SetI( go:Flag() );
			data:Offset( data, 4 );
			data:AppendArray( mat_world, 16 );
			graphics.UnmapBuffer();
			local playing = animctrl:Apply( temp );
			temp = blackboard.dt;
			color_renderer:DrawIndexedInstanced( icount, 1 );
		end
	------------------------------------------------------------- depth only
		InstFuncDepth = function()
			go:Transform(transform);
			go:AnimCtrl( animctrl );
			if ( ttt == 0 ) then animctrl:Switch(animations.tadanim); ttt= 1; end
			transform:BuildMatrix( mat_world );
			graphics.MapBuffer( data, inst_vb );
			data:Offset( data, 4 );
			data:AppendArray( mat_world, 16 );
			graphics.UnmapBuffer();
			animctrl:Apply( temp );
			temp = 0.0;
			depth_renderer:DrawIndexedInstanced( icount, 1 );
		end
	else
	----------------------------------------------------------------------------------- functions for updating static models
		InstFunc = function()
			go:Transform(transform);
			transform:BuildMatrix( mat_world );
			data:SetI( go:Flag() );
			data:Offset( data, 4 );
			data:AppendArray( mat_world, 16 );
			num_insts = num_insts + 1;
		end
	end
	----------------------------------------------------------------------------------- system update function
	local SysFunc;
	local SysFuncDepth;
	------------------------------------------------------------------- animated
	if ( animated == 1 ) then
		SysFunc = function()
			mat:ForEachObject( go, InstFunc );
		end
		SysFuncDepth = function()
			mat:ForEachObject( go, InstFuncDepth );
		end
	------------------------------------------------------------------- static
	else
		SysFunc = function()
			graphics.MapBuffer( data, inst_vb );
			num_insts = 0;
			mat:ForEachObject( go, InstFunc );
			graphics.UnmapBuffer();

			if ( num_insts > 0 ) then
				color_renderer:DrawIndexedInstanced( icount, num_insts );
			end
		end
		SysFuncDepth = function()
			graphics.MapBuffer( data, inst_vb );
			num_insts = 0;
			mat:ForEachObject( go, InstFunc );
			graphics.UnmapBuffer();
			if ( num_insts > 0 ) then
				depth_renderer:DrawIndexedInstanced( icount, num_insts );
			end
		end
	end
	----------------------------------------------------------------------------------- main loop
	while true do
		local matrix, method = coroutine.yield();
		graphics.MapBuffer( data, frame_cb );
		data:AppendArray( matrix, 16 );
		graphics.UnmapBuffer();
		
		if ( method == gu.render_diffuse ) then
			SysFunc();
		else
			SysFuncDepth();
		end

	end
end