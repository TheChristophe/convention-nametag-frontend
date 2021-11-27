import React from 'react';
import { Container, createTheme, Stack, ThemeProvider } from '@mui/material';
import './actionable.css';
import { VideoUpload } from './VideoUpload';
import { VideoMetadata } from './VideoMetadata';
import { VideoEntry } from './VideoEntry';

const theme = createTheme();

const existing: VideoMetadata[] = [
    {
        thumbnailUrl: 'https://i.imgur.com/C3QGaPB.jpeg',
        filename: 'cats.mp4',
        duration: '144:00:00',
    },
    {
        thumbnailUrl: 'https://i.imgur.com/rpQdRoY.jpeg',
        filename: 'dogs.mp4',
        duration: '0:00:07',
    },
];

type Videos = {
    videos: VideoMetadata[];
};

function App() {
    const existingVideos: Videos = { videos: existing };

    return (
        <ThemeProvider theme={theme}>
            <Container maxWidth="sm" sx={{ my: '2em' }}>
                <VideoUpload />
                <hr style={{ border: 0, height: '1px', background: '#CCC' }} />
                <Stack spacing={2}>
                    {existingVideos.videos.map((video) => (
                        <VideoEntry key={video.filename} metadata={video} />
                    ))}
                </Stack>
            </Container>
        </ThemeProvider>
    );
}

export default App;
