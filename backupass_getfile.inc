<?php

function backupass_getfile()
{
    //$image_uri = system_retrieve_file('http://ipsumimage.appspot.com/140x100.png'); 
    if(isset($_GET["fileid"]) && isset($_GET["version"]))
    {
        $sql = db_select("ba_file_archive","a");
        $sql->fields("a",array("fileid","version","path"));
        $sql->condition("a.fileid",$_GET["fileid"],"=");
        $sql->condition("a.version",$_GET["version"],"=");
        $result = $sql->execute();
        if($result->rowCount() > 0)
        {
            $row = $result->fetchObject();
            $filepath = $row->path;
            if (file_exists($filepath) )
            {
                //drupal_set_message("Normal Code.");
                $filepath= $filepath;
            }
            else if (file_exists(@mb_convert_encoding($filepath, 'big-5')) )
            {
                //drupal_set_message("big-5 Code.");
                $filepath= mb_convert_encoding($filepath, 'big-5');
            }
            else if (file_exists(@mb_convert_encoding($filepath, 'UTF-8')) )
            {
                //drupal_set_message("utf-8 Code.");
                $filepath= mb_convert_encoding($filepath, 'UTF-8');
            }
            else if (file_exists(@mb_convert_encoding($filepath, 'GB2312')) )
            {
                //drupal_set_message("GB2312 Code.");
                $filepath= mb_convert_encoding($filepath, 'GB2312'); 
            }
            $zip = new ZipArchive;
            if ($zip->open($filepath) === TRUE) {
                $filename = $zip->getNameIndex(0);                
                $fileinfo = pathinfo($filename);
                copy("zip://".$filepath."#".$filename, file_directory_temp()."/".$fileinfo['basename']);
                $zip->close();
                transferFile(file_directory_temp()."/".$fileinfo['basename']);
                //drupal_set_message(file_directory_temp()."/".$fileinfo['basename']);
                unlink(file_directory_temp()."/".$fileinfo['basename']);
                drupal_exit();
            } else {
                drupal_set_message('failed');
            }
        }
    }
    else if(isset($_GET["fileid"])){
        $sql = db_select("ba_filelist","f");
        $sql->fields("f",array("fileid","filename","sourcepath","parentpath","filepath"));
        $sql->condition("f.fileid",$_GET["fileid"],"=");
        $result = $sql->execute();
        if($result->rowCount() > 0)
        {
            $row = $result->fetchObject();
            $filename = $row->filepath;
            $temp_path = variable_get('backupass_backup_path', 'C:\ShareFolder');       
            $temp_path = realpath($temp_path).$row->parentpath."/";
            $filepath = $temp_path . $filename;
            //drupal_set_message($filepath);
            //drupal_set_message(mb_detect_encoding($filepath, "auto"));

            if (file_exists($filepath) )
            {
                //drupal_set_message("Normal Code.");
                $filepath= $filepath;
            }
            else if (file_exists(@mb_convert_encoding($filepath, 'big-5')) )
            {
                //drupal_set_message("big-5 Code.");
                $filepath= mb_convert_encoding($filepath, 'big-5');
            }
            else if (file_exists(@mb_convert_encoding($filepath, 'UTF-8')) )
            {
                //drupal_set_message("utf-8 Code.");
                $filepath= mb_convert_encoding($filepath, 'UTF-8');
            }
            else if (file_exists(@mb_convert_encoding($filepath, 'GB2312')) )
            {
                //drupal_set_message("GB2312 Code.");
                $filepath= mb_convert_encoding($filepath, 'GB2312'); 
            }

            transferFile($filepath);
            drupal_exit();
        }
        else {
            drupal_set_message("File not found", 'error');
        }
    }
// $image = image_load($image_uri); 
// drupal_set_message('<pre>' . print_r($image, true) . '</pre>');
return '';
}


function transferFile($filepath)
{
        //drupal_set_message(transliteration_clean_filename($filepath));
    //drupal_set_message($temp_path . $filename);
    if (file_exists($filepath) ) {
        // Serve file download.
        drupal_add_http_header('Pragma', 'public');
        drupal_add_http_header('Expires', '0');
    // 	  drupal_add_http_header('Cache-Control', 'must-revalidate, post-check=0, pre-check=0');
        drupal_add_http_header('Content-Type', 'application/download');
        drupal_add_http_header('Content-Disposition', 'attachment; filename=' . basename($filepath) . ';');
        //drupal_add_http_header('Content-Transfer-Encoding', 'binary');
        drupal_add_http_header('Content-Length', filesize($filepath));
        readfile($filepath);
    // 	  unlink($temp_path . $filename);
    } else
    {
            drupal_set_message("File not exist:".$temp_path . $filename, 'error');
    }
}
?>