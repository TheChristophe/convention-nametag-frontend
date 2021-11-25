import React, { useState } from 'react';
import { ThumbnailPaper } from './ThumbnailPaper';
import { Box, Button } from '@mui/material';
import { makeThumbnailUrl } from './utility';

const videoFormats = [
    'image/gif', // animated .gif
    'video/mp4', // .mp4
    'video/webm', // .webm
    // the following could be dropped
    'video/quicktime', // .mov
    'video/x-msvideo', // .avi
    'video/x-ms-wmv', // .wmv
];

export function VideoUpload() {
    const [thumbnailUrl, setThumbnailUrl] = useState<string>('');
    const [file, setFile] = useState<File | undefined>();

    return (
        <ThumbnailPaper thumbnailUrl={thumbnailUrl}>
            <Box sx={{ padding: '2em' }}>
                <Button variant="contained" component="label">
                    Select
                    <input
                        type="file"
                        hidden
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
                </Button>{' '}
                {file?.name}{' '}
                {file && (
                    <Button variant="contained" component="label">
                        Submit
                    </Button>
                )}
            </Box>
        </ThumbnailPaper>
    );
}
