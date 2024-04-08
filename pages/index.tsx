import React from 'react';
import { useQuery } from '@tanstack/react-query';
import type { NextPage } from 'next';
import VideoUpload from 'components/VideoUpload';
import VideoEntry from 'components/VideoEntry';
import type VideoMetadata from 'components/VideoMetadata';
import { HOST } from 'components/config';

type Videos = {
    videos: VideoMetadata[];
};

const Home: NextPage = () => {
    const existingVideos = useQuery({
        queryKey: ['videos'],
        queryFn: () =>
            fetch(HOST + '/videos').then((res): Promise<Videos> => {
                if (!res.ok) {
                    throw new Error(res.statusText);
                }
                return res.json();
            }),
    });

    return (
        <div className="container mx-auto flex justify-center items-center min-h-[100svh]">
            <div className="mx-auto max-w-[80ch]">
                <VideoUpload reload={() => existingVideos.refetch()} className="mb-2" />

                <div className="flex flex-col gap-2">
                    {existingVideos.data?.videos.map((video) => (
                        <VideoEntry
                            key={video.filename}
                            metadata={video}
                            reload={() => existingVideos.refetch()}
                        />
                    ))}
                </div>
            </div>
        </div>
    );
};

export default Home;
