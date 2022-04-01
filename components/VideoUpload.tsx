import React, { useState } from 'react';
import { Box, Button, Grid, Paper } from '@mui/material';
import { makeThumbnailUrl } from './utility';
import { useMutation } from 'react-query';
import { HOST } from './config';
import { Thumbnail } from './VideoEntry';

const videoFormats = [
    'image/gif', // animated .gif
    'video/mp4', // .mp4
    'video/webm', // .webm
    // the following could be dropped
    'video/quicktime', // .mov
    'video/x-msvideo', // .avi
    'video/x-ms-wmv', // .wmv
];

export function VideoUpload({ reload }: { reload: () => void }) {
    const [thumbnailUrl, setThumbnailUrl] = useState<string>('');
    const [file, setFile] = useState<File | undefined>();

    const mutation = useMutation(
        (data: File) => {
            return fetch(HOST + '/videos/' + data.name, {
                method: 'POST',
                mode: 'no-cors',
                cache: 'no-cache',
                headers: { 'Content-Type': data.type },
                body: data,
            });
        },
        {
            onSuccess: () => {
                setFile(undefined);
                reload();
            },
        }
    );

    function upload() {
        if (file !== undefined) {
            mutation.mutate(file);
        }
    }

    return (
        <Paper elevation={4}>
            <Grid container width="100%">
                <Grid item xs={6} md={4}>
                    <Thumbnail thumbnailUrl={thumbnailUrl} />
                </Grid>
                <Grid item xs={6} md={8}>
                    <div
                        style={{
                            position: 'relative',
                        }}
                    >
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
                            <Button
                                variant="contained"
                                component="label"
                                onClick={upload}
                                disabled={file === undefined || mutation.isLoading}
                            >
                                Submit
                            </Button>
                            <br />
                            {file?.name}
                            <br />
                        </Box>
                    </div>
                </Grid>
            </Grid>
        </Paper>
    );
}
