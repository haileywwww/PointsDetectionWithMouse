
dirname = "./save0524_calib/up/";
saveddepthdir = "./save0524_calib/up/data/camera1/board_depth_filled/";
width = 640
height = 480

num = 1;
for index = 0:100
    IrFileName = append(dirname, "Ir_", int2str(index), "_0.png");
    if exist(IrFileName,'file')
        index
        num
        IrFileName
        DepthFileName = append(dirname, "Raw_Depth_", int2str(index), "_0.txt");
        DepthFileName

        try
            imgRgb = imread(IrFileName);
            imgDepthRaw_txt = load(DepthFileName);
    
            imgDepthRaw = zeros(height, width); 

            for i = 1:1:height 
                for j = 1:1:width
                    imgDepthRaw(i, j) = imgDepthRaw_txt((i-1) * width + (j-1) + 1);
                end
            end
    
            % img_resize = imresize(imgDepthRaw, [1280,1706]);
            % img_crop = imcrop(img_resize, [313, 0, 1080, 1280]);
            % img_flip = flip(img_crop, 2);
    
             
            figure(1);
            % Show the RGB image.
            subplot(1,4,1);
            imagesc(imgRgb);
            axis off;
            axis equal;
            title('RGB');
            
            % Show the Raw Depth image.
            subplot(1,4,2);
            imagesc(imgDepthRaw);
            axis off;
            axis equal;
            title('Raw Depth');
            caxis([0 5000]);
            
            denoisedDepthImg = fill_depth_colorization(imgRgb, imgDepthRaw, 1);
            % Imgout = denoisedDepthImg(:,123:518);%imcrop(denoisedDepthImg,[122 0 395 480])
            % writeImage = imresize(Imgout,[1280,1056]);
            subplot(1,4,4);
            imagesc(denoisedDepthImg);
            axis off;
            axis equal;
            title('Projected Depth');
            caxis([0 5000]);
            
            % pause(0.01);
            
            writefname = append(saveddepthdir, "Raw_Depth_", int2str(index), "_completion.raw");
            fid=fopen(writefname, 'wb');
            fwrite(fid, denoisedDepthImg.','float32');
            fclose(fid);
            % writeimagename = append(saveddepthdir, int2str(num), ".png");
            % imwrite(denoisedDepthImg, writeimagename)
        catch
            disp(['error file name: ',DepthFileName])
    
        end

        num = num + 1;
    end

end


