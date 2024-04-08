import { useState } from 'react';
import { makeThumbnailUrl } from './utility';
import { useMutation } from '@tanstack/react-query';
import { HOST } from './config';
import { Thumbnail } from './VideoEntry';
import clsx from 'clsx';

const videoFormats = [
    'image/gif', // animated .gif
    'video/mp4', // .mp4
    'video/webm', // .webm
    // the following could be dropped
    'video/quicktime', // .mov
    'video/x-msvideo', // .avi
    'video/x-ms-wmv', // .wmv
];

type VideoUploadProps = { reload: () => void; className?: string };
const VideoUpload = ({ reload, className }: VideoUploadProps) => {
    const [thumbnailUrl, setThumbnailUrl] = useState<string>('');
    const [file, setFile] = useState<File | undefined>();

    const mutation = useMutation({
        mutationFn: (data: File) =>
            fetch(HOST + '/videos/' + data.name, {
                method: 'POST',
                mode: 'no-cors',
                cache: 'no-cache',
                headers: { 'Content-Type': data.type },
                body: data,
            }),
        onSuccess: () => {
            setFile(undefined);
            reload();
        },
    });

    function upload() {
        if (file !== undefined) {
            mutation.mutate(file);
        }
    }

    return (
        <div className={clsx('border flex flex-row rounded shadow', className)}>
            <div>
                <Thumbnail thumbnailUrl={thumbnailUrl} />
            </div>
            <div>
                <div className="p-2">
                    <input
                        type="file"
                        //hidden
                        accept={videoFormats.join(',')}
                        onChange={(event) => {
                            if (
                                event.target &&
                                event.target.files &&
                                event.target.files.length > 0
                            ) {
                                const file = event.target.files[0];
                                setFile(file);
                                makeThumbnailUrl(file, setThumbnailUrl);
                            }
                        }}
                    />
                    <button
                        type="submit"
                        onClick={upload}
                        disabled={file === undefined || mutation.isPending}
                        className="disabled:cursor-not-allowed"
                    >
                        Upload
                    </button>
                </div>
            </div>
        </div>
    );
};

export default VideoUpload;
