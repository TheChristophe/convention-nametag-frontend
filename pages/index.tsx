import React from 'react';
import { Container, createTheme, Stack, ThemeProvider } from '@mui/material';
import { useQuery } from 'react-query';
import type { NextPage } from 'next';
import { VideoUpload } from '../components/VideoUpload';
import { VideoEntry } from '../components/VideoEntry';
import { VideoMetadata } from '../components/VideoMetadata';
import { HOST } from '../components/config';

const theme = createTheme();

type Videos = {
    videos: VideoMetadata[];
};

const Home: NextPage = () => {
    const existingVideos = useQuery('videos', () =>
        fetch(HOST + '/videos').then((res): Promise<Videos> => {
            if (!res.ok) {
                throw new Error(res.statusText);
            }
            return res.json();
        })
    );

    return (
        <ThemeProvider theme={theme}>
            <Container maxWidth="sm" sx={{ my: '2em' }}>
                <VideoUpload reload={() => existingVideos.refetch()} />
                <hr style={{ border: 0, height: '1px', background: '#CCC' }} />
                <Stack spacing={2}>
                    {existingVideos.data?.videos.map((video) => (
                        <VideoEntry
                            key={video.filename}
                            metadata={video}
                            reload={() => existingVideos.refetch()}
                        />
                    ))}
                </Stack>
            </Container>
        </ThemeProvider>
    );
};

export default Home;
